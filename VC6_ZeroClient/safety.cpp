#include "safety.h"
#include <stdio.h>

char debug_name[5][100]={"idaq.exe","idaq64.exe","VMwareTray.exe","SbieSvc.exe","OllyDBG.exe"};
char path_name[7][100]={"debug","virus","temp","ida","olly","disasm","hack"};


bool agree_control();  //是否同意被控制
bool is_run();        //是否已经运行
bool is_debug();      //是否进程中存在debug进程
bool is_path_safe();  //是否位置安全

bool find_debuger(const char *processname);   //比较进程列表是否存在调试器进程
bool find_virus_path(const char *processname);  //位置字符串比较

int isSafety();

bool agree_control()
{
	if(IDNO == MessageBox(NULL,"Really want to be controlled?","!!!!!",MB_YESNO))
    {
        return true;  //询问被控制人是否愿意  
    }
	return false;
}

bool is_run()
{
	CreateMutexA(NULL,FALSE,"ZeroNet");
    if(GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cout << "Same program already running" << std::endl;
        return true;   //查询是否已运行
    }
	return false;
}

bool find_debuger(const char *processname)
{
	for(int i=0;i<5;i++)
	{
		char *s=debug_name[i];
		while(*processname!='\0' && *processname==*s)
		{
			processname++;
			s++;
		}
		if(*processname=='\0' && *s=='\0'){
			return true;
		}
	}
	return false;
}


bool is_debug()
{
	//多种反调试技术   SEH+IsDebuggerPresent+.....+全路径检测+进程遍历
	int IsSafe=0;
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(pe32);
	HANDLE hprocesssnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(INVALID_HANDLE_VALUE!=hprocesssnapshot)
	{
		BOOL bprocess=Process32First(hprocesssnapshot,&pe32);
		while(bprocess)
		{
			if(find_debuger(pe32.szExeFile))
			{
				IsSafe++;
			}
		    bprocess=Process32Next(hprocesssnapshot,&pe32);
		}
	}
	if(IsSafe){
		return true;
	}
	else{
		return false;
	}
}

bool is_path_safe(){
	int IsSafe=0;
	char filename[MAX_PATH];
	if(GetModuleFileName(NULL,filename,MAX_PATH))
	{
		if(find_virus_path(filename))
		{
			IsSafe++;
		}
	}
	if(IsSafe)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool find_virus_path(const char *processname)
{
	const char *ori=processname;
	for(int i=0;i<7;i++)
	{
		processname=ori;
		while(*processname!='\0')
		{
			char *s=path_name[i];
			while(*processname!=*s && *processname!=(*s)-32 && *processname!='\0')
			{
				processname++;
			}
			while(*processname!='\0' && (*processname==*s || *processname==(*s)-32))
			{
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



int isSafety()
{
	int IsSafe=0;

	if(agree_control()   //拒绝
		||is_run()
		||is_debug()
		||is_path_safe()
		/*||IsDebuggerPresent()*/
		)
	{  
        IsSafe++;  //询问被控制人是否愿意
    }
    return IsSafe;
}



