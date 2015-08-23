#include "ztWorkToolRepeater.h"


/*变量申明*/
INT32 g_i32RunAppTime = 0;				// 进程执行时间
char g_szRunAppName[256] = "";			// 需要执行的进程名
DWORD g_dwCurRunPID = 0;


/*函数实现*/
unsigned __stdcall ExecuteLogicThread(void *pAnything)
{
	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	// 线程循环
	while (TRUE)
	{
		EnterCriticalSection(&g_csAppCommonMutex);
		if (g_bIsExitThisProcess == TRUE)
		{
			g_i32ThreadCounter--;
			LeaveCriticalSection(&g_csAppCommonMutex);
			break;
		}
		LeaveCriticalSection(&g_csAppCommonMutex);

		// 等待启动信息的获取
		WaitForSingleObject(g_hCanCreateProcessEvt, INFINITE);

		// 创建进程
		static UINT32 ui32CreateTime = 0;
		if (ui32CreateTime == 0)										// 开始运行
		{
			// 输出
			char szOutput[256] = "";
			static INT32 i32RunTimes = 0;
			SendMsgToOutputWnd("\n", FALSE);
			sprintf(szOutput, "Run times: %d\n", ++i32RunTimes);
			SendMsgToOutputWnd(szOutput, TRUE);

			if (pi.hProcess != NULL && pi.hProcess != INVALID_HANDLE_VALUE)
				CloseHandle(pi.hProcess);

			if (pi.hThread != NULL && pi.hThread != INVALID_HANDLE_VALUE)
				CloseHandle(pi.hThread);

			STARTUPINFO si;
			memset(&si, 0, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES;
			si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
			si.hStdError = GetStdHandle(STD_ERROR_HANDLE);	
			si.wShowWindow = SW_SHOW;
			si.lpTitle = NULL;
			si.dwFlags = STARTF_USESTDHANDLES| STARTF_USESHOWWINDOW;
			EnterCriticalSection(&g_csAppCommonMutex);
			if (CreateProcess(g_szRunAppName, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi) == FALSE)
			{
				LeaveCriticalSection(&g_csAppCommonMutex);
				continue;
			}
			g_dwCurRunPID = pi.dwProcessId;
			LeaveCriticalSection(&g_csAppCommonMutex);
			ui32CreateTime = GetTickCount();
		}

		EnterCriticalSection(&g_csAppCommonMutex);
		if (GetTickCount() - ui32CreateTime > (UINT32)g_i32RunAppTime)	// 运行结束
		{
			ui32CreateTime = 0;
			ExitProcessTree(TRUE, pi.dwProcessId);
			CloseHandle(pi.hProcess);
			LeaveCriticalSection(&g_csAppCommonMutex);
			continue;
		}
		else															// 运行中
		{
			char szOutput[256] = "";
			UINT32 ui32CurTickCount = GetTickCount();
			SendMsgToOutputWnd("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", FALSE);
			sprintf(szOutput, "RunTime: %05.1fs RemainderTime:%05.1fs", 
				(float)(ui32CurTickCount - ui32CreateTime)/1000.0f, 
				(float)(ui32CreateTime + g_i32RunAppTime - ui32CurTickCount)/1000.0f);
			SendMsgToOutputWnd(szOutput, TRUE);
		}

		LeaveCriticalSection(&g_csAppCommonMutex);
		Sleep(50);
	}

	return 0;
}

BOOL IsFileExist(char *szFileName)
{
	FILE *pFile = fopen(szFileName, "r");
	if (pFile == NULL)
		return FALSE;
	fclose(pFile);
	return TRUE;
}