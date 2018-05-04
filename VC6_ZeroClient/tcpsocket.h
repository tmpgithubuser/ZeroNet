#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <winsock2.h>
#include <iostream>
#include <string>
#include <cstdio>

class TcpSocket
{
public:
    TcpSocket();

    static std::string fromDomainToIP(std::string domain);

    bool connectTo(std::string domain, int port);
    void dissconnect();
    bool sendData(const char *data, unsigned int size);
    int recvData(char *data, int size);

    bool isConnected() {
        return (int)mSock != SOCKET_ERROR;
    }

    std::string mIp;
    int mPort;

private:
    SOCKET mSock;
    struct sockaddr_in mAddr;
};

#endif // TCPSOCKET_H
