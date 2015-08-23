#pragma once
#include <windows.h>
#include <psapi.h>
#include <Tlhelp32.h>
#include <stdio.h>
#include <process.h>

#pragma comment(lib, "psapi.lib")

#pragma warning(disable: 4996)


/*函数申明*/
BOOL WriteAppLog(char *szLog);				// 写日志
BOOL DestroyRes(void);						// 资源销毁
BOOL AppRunning(void);						// 主循环，运行函数
BOOL AnalyseCmd(char *szCmd);				// 命令分析
BOOL IsFileExist(char *szFileName);			// 检查文件是否存在
BOOL InitData(int argc, char* argv[]);		// 数据初始化
BOOL SendMsgToOutputWnd(char *szBuf， , BOOL bShowTime);	// 发送消息到输出窗口
unsigned __stdcall ExecuteLogicThread(void *pAnything);		// 逻辑线程
BOOL ExitProcessTree(BOOL bIsExitItself, DWORD dwThisProcessID);	// 结束进程树
BOOL GetFileNameAndPathByFullName(char __out *pFileName, char __out *pFilePath, char __in *pFileFullName);


/*外部变量声明*/
extern DWORD g_dwCurRunPID;
extern INT32 g_i32RunAppTime;
extern CHAR g_szAppCfgName[256];				
extern CHAR g_szAppLogName[256];				
extern BOOL g_bIsExitThisProcess;
extern INT32 g_i32ThreadCounter;
extern char g_szRunAppName[256];

extern HANDLE g_hCanCreateProcessEvt;

extern CRITICAL_SECTION g_csAppLogMutex;		
extern CRITICAL_SECTION g_csAppCommonMutex;		
