#include "myMultiThreadTest.h"

LARGE_INTEGER g_i64BeginTime;
LARGE_INTEGER g_i64CPUFreq;
HANDLE g_hAskEvt = INVALID_HANDLE_VALUE;

int main(int argc, char *argv[])
{
	g_hAskEvt = CreateEvent(NULL, FALSE, FALSE, "TESTER");
	QueryPerformanceFrequency(&g_i64CPUFreq);
	_beginthreadex(NULL, 0, RunCreateThreadFunc, NULL, 0, NULL);
	while(TRUE)
	{
		QueryPerformanceCounter(&g_i64BeginTime);
		SetEvent(g_hAskEvt);
		Sleep(100);
	}

	return 0;

	// 数据初始化
	InitAppData();
	if (_beginthreadex(NULL, 0, RunCreateThreadFunc, NULL, 0, NULL))
	{
		EnterCriticalSection(&g_csThreadCtrlLock);
		g_i32ThreadCounter++;
		LeaveCriticalSection(&g_csThreadCtrlLock);

		while (TRUE)
		{
			static INT32 i32Index = 0;
			i32Index = (++i32Index >= MAXTHREADCOUNT ? 0 : i32Index);
			printf("Output: %d 0x%08x %d\n", i32Index, g_sThreadInfo[i32Index].m_hThreadHandle, ++g_sThreadInfo[i32Index].m_i32OutputNum);
			//char szInput[512] = "";
			//scanf("%s", szInput);
			//printf(szInput);
			//if (strcmp("exit", szInput) == 0)
			//{
			//	EnterCriticalSection(&g_csThreadCtrlLock);
			//	g_bIsExitMainThread = TRUE;
			//	LeaveCriticalSection(&g_csThreadCtrlLock);
			//	break;
			//}
		}

		while (TRUE)
		{
			EnterCriticalSection(&g_csThreadCtrlLock);
			if (g_i32ThreadCounter == 0)
			{
				LeaveCriticalSection(&g_csThreadCtrlLock);
				break;
			}
			LeaveCriticalSection(&g_csThreadCtrlLock);
			Sleep(10);
		}
	}
	
	return 0;
}

unsigned __stdcall RunTestThreadFunc(void *pParam)
{
	STRU_THREADINFO *pstruThisThreadInfo = (STRU_THREADINFO*)pParam;

	while (TRUE)
	{
		// 判断主线程是否退出，若退出立即跳出循环
		EnterCriticalSection(&g_csThreadCtrlLock);
		if (g_bIsExitMainThread == TRUE)
		{
			LeaveCriticalSection(&g_csThreadCtrlLock);
			break;
		}
		LeaveCriticalSection(&g_csThreadCtrlLock);

		pstruThisThreadInfo->m_i32OutputNum++;
		Sleep(50);
	}

	// 线程退出计数器减1
	EnterCriticalSection(&g_csThreadCtrlLock);
	g_i32ThreadCounter--;
	LeaveCriticalSection(&g_csThreadCtrlLock);

	return 0;
}

unsigned __stdcall RunCreateThreadFunc(void *pParam)
{
	while (TRUE)
	{
		LARGE_INTEGER i64EndTime;
		WaitForSingleObject(g_hAskEvt, INFINITE);
		QueryPerformanceCounter(&i64EndTime);
		INT64 i64Temp1 = i64EndTime.QuadPart-g_i64BeginTime.QuadPart;
		INT64 i64Temp2 = i64Temp1 * 1000 * 1000 * 1000;
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bCost time: %I64dns!", i64Temp2 / g_i64CPUFreq.QuadPart);
	}

	return 0;

	INT8 _i;

	Sleep(1000);
	EnterCriticalSection(&g_csThreadCtrlLock);
	for (_i = 0; _i < MAXTHREADCOUNT; _i++)
	{
		// 判断主线程是否退出，若退出立即跳出循环
		
		if (g_bIsExitMainThread == TRUE)
		{
			break;
		}

		if (g_sThreadInfo[_i].m_bIsThreadCreate == FALSE)
		{
			g_sThreadInfo[_i].m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, RunTestThreadFunc, (void*)(&g_sThreadInfo[_i]), CREATE_SUSPENDED, &g_sThreadInfo[_i].m_ui32ThreadID);
			if (g_sThreadInfo[_i].m_hThreadHandle != 0)
			{
				g_i32ThreadCounter++;
				ResumeThread(g_sThreadInfo[_i].m_hThreadHandle);
			}
		}
		Sleep(50);
	}

	printf("%6s%6s%11s%6s\n", "index", "id", "handle", "flag");
	for (_i = 0; _i < MAXTHREADCOUNT; _i++)
	{
		// 判断主线程是否退出，若退出立即跳出循环
		if (g_bIsExitMainThread == TRUE)
		{
			break;
		}

		char szOutput[128] = "";
		if (g_sThreadInfo[_i].m_bIsThreadCreate == TRUE)
		{
			sprintf(szOutput, "%6d%6d 0x%08x%6s\n", 
				g_sThreadInfo[_i].m_i32ThreadIndex,
				g_sThreadInfo[_i].m_ui32ThreadID,
				(UINT32)g_sThreadInfo[_i].m_hThreadHandle,
				"TRUE");
		}
		else
		{
			sprintf(szOutput, "%6d%6d 0x%08x%6s\n", 
				g_sThreadInfo[_i].m_i32ThreadIndex,
				g_sThreadInfo[_i].m_ui32ThreadID,
				(UINT32)g_sThreadInfo[_i].m_hThreadHandle,
				"FALSE");
		}
		printf(szOutput);
	}
	LeaveCriticalSection(&g_csThreadCtrlLock);

	while (TRUE)
	{
		// 判断主线程是否退出，若退出立即跳出循环
		EnterCriticalSection(&g_csThreadCtrlLock);
		if (g_bIsExitMainThread == TRUE)
		{
			LeaveCriticalSection(&g_csThreadCtrlLock);
			break;
		}
		LeaveCriticalSection(&g_csThreadCtrlLock);

		//char szOutput[256] = "";
		//for (_i = 0; _i < MAXTHREADCOUNT*9; _i++)
		//	printf("\b");
		//for (_i = 0; _i < MAXTHREADCOUNT; _i++)
		//{
		//	char szTemp[256] = "";
		//	sprintf(szTemp, "%s %08d", szOutput, g_sThreadInfo[_i].m_i32OutputNum--);
		//	strcpy(szOutput, szTemp);
		//}
		//printf(szOutput);
		Sleep(30);
	}
	
	// 线程退出计数器减1
	EnterCriticalSection(&g_csThreadCtrlLock);
	g_i32ThreadCounter--;
	LeaveCriticalSection(&g_csThreadCtrlLock);
	return 0;
}

void InitAppData(void)
{
	INT32 _i;
	g_i32ThreadCounter = 0;
	g_bIsExitMainThread = FALSE;
	InitializeCriticalSection(&g_csThreadCtrlLock);

	for (_i = 0; _i < MAXTHREADCOUNT; _i++)
	{
		g_sThreadInfo[_i].clear();
		g_sThreadInfo[_i].m_i32ThreadIndex= _i;
	}
}


/*
亲爱的小城堡，今天2013年1月4号是我们相恋的第251天。
在过去的那些日子里，你笑过哭过委屈过我浑过错过小气过。
从你的不排斥到你的接受再到你的喜欢，每一步的前移我们都一起努力一起坚持着。
我们平淡着但我们幸福着。
*/
