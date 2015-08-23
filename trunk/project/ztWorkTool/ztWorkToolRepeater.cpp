#include "ztWorkToolRepeater.h"


/*��������*/
#define SENDBUFSIZE (1024*20)
CHAR g_szAppCfgName[256] = "";				// �����������ļ���
CHAR g_szAppLogName[256] = "";				// ��������־�ļ���
CHAR g_szThisAppPath[256] = "";				// ����������·��
BOOL g_bIsExitThisProcess = FALSE;			// �����˳���־
INT32 g_i32ThreadCounter = -1;				// �̼߳�����

HANDLE g_hCanCreateProcessEvt = INVALID_HANDLE_VALUE;	// ���Դ��������¼�
HANDLE g_hNamedPipeToOutputWnd = INVALID_HANDLE_VALUE;	// ������ṩ������ܵ�

CRITICAL_SECTION g_csAppLogMutex;			// ��־��
CRITICAL_SECTION g_csAppCommonMutex;		// ������


/*����ʵ��*/
int main(int argc, char* argv[])
{
	if (InitData(argc, argv) == FALSE)
		return 1;

	if (AppRunning() == FALSE)
		return 2;

	if (DestroyRes() == FALSE)
		return 3;

	return 0;
}

BOOL AppRunning(void)
{
	// ��ѭ��
	while (TRUE)
	{
		EnterCriticalSection(&g_csAppCommonMutex);
		if (g_bIsExitThisProcess == TRUE)
		{
			SendMsgToOutputWnd("exit", FALSE);
			ExitProcessTree(TRUE, g_dwCurRunPID);
			LeaveCriticalSection(&g_csAppCommonMutex);
			break;
		}
		LeaveCriticalSection(&g_csAppCommonMutex);

		// ��֤�ļ���
GETFILENAME:
		if (strstr(g_szRunAppName, "ERROR") != NULL || IsFileExist(g_szRunAppName) == FALSE)
		{
			ResetEvent(g_hCanCreateProcessEvt);
			printf("Please enter app name:\n");
			scanf("%s", g_szRunAppName);
			if (IsFileExist(g_szRunAppName) == FALSE)
			{
				printf("%s is not exist!\n", g_szRunAppName);
				strcpy(g_szRunAppName, "ERROR");
				goto GETFILENAME;
			}

		}
GETRUNTIME:
		if (g_i32RunAppTime == 0)
		{
			ResetEvent(g_hCanCreateProcessEvt);
			printf("Please enter app run time(ms):\n");
			scanf("%d", &g_i32RunAppTime);
			if (g_i32RunAppTime == 0)
				goto GETRUNTIME;
		}

		SetEvent(g_hCanCreateProcessEvt);

		char szInput[512] = "";
		scanf("%[^\n]", szInput);
		fflush(stdin);
		AnalyseCmd(szInput);
	}

	return TRUE;
}

// �������
BOOL AnalyseCmd(char *szCmd)
{
	if (*szCmd == NULL)	return FALSE;

	char szOutput[512] = "";
	sprintf(szOutput, "Input: %s\n", szCmd);
	WriteAppLog(szOutput);

	if (stricmp(szCmd, "exit") == 0)									// �˳�
	{
		EnterCriticalSection(&g_csAppCommonMutex);
		g_bIsExitThisProcess = TRUE;
		LeaveCriticalSection(&g_csAppCommonMutex);
		WriteAppLog("EXIT!!! \n\n");
		return TRUE;
	}
	else if (strstr(szCmd, "set time "))								// ����ʱ��
	{
		EnterCriticalSection(&g_csAppCommonMutex);
		sscanf(szCmd, "set time %d", &g_i32RunAppTime);
		LeaveCriticalSection(&g_csAppCommonMutex);
		return TRUE;
	}

	system(szCmd);
	return TRUE;
}

BOOL InitData(int argc, char* argv[])
{
	char *szFindPos = NULL;
	char szModuleName[256] = "";
	GetModuleFileNameEx(GetCurrentProcess(), NULL, szModuleName, sizeof(szModuleName));

	// ��ȡ��־�ļ���
	szFindPos = strstr(szModuleName, ".exe");
	if (szFindPos == NULL)
		return FALSE;
	*(szFindPos+1) = 'l';
	*(szFindPos+2) = 'o';
	*(szFindPos+3) = 'g';
	*(szFindPos+4) = 0;
	strcpy(g_szAppLogName, szModuleName);

	// ��ȡ�����ļ���
	szFindPos = strstr(szModuleName, ".log");
	if (szFindPos == NULL)
		return FALSE;
	*(szFindPos+1) = 'i';
	*(szFindPos+2) = 'n';
	*(szFindPos+3) = 'i';
	*(szFindPos+4) = 0;
	strcpy(g_szAppCfgName, szModuleName);

	// ��ʼ���ٽ���
	InitializeCriticalSection(&g_csAppLogMutex);
	InitializeCriticalSection(&g_csAppCommonMutex);

	// �����¼�
	g_hCanCreateProcessEvt = CreateEvent(NULL, TRUE, FALSE, "CANCREATEPROCESSEVT");

	// ��ȡ�����ļ���Ϣ
	GetPrivateProfileString("RunInfo", "RunName", "ERROR", g_szRunAppName, sizeof(g_szRunAppName), g_szAppCfgName);
	g_i32RunAppTime = GetPrivateProfileInt("RunInfo", "RunTime", 0, g_szAppCfgName);

	// ����ִ���߼��߳�
	if (_beginthreadex(NULL, NULL, ExecuteLogicThread, NULL, 0, NULL) == 0)
	{
		char szOutput[128] = "";
		sprintf(szOutput, "Create thread ExecuteLogicThread failed! Error code is %u", GetLastError());
		SendMsgToOutputWnd(szOutput, TRUE);
		return FALSE;
	}

	EnterCriticalSection(&g_csAppCommonMutex);
	if (g_i32ThreadCounter == -1)
		g_i32ThreadCounter = 1;
	else
		g_i32ThreadCounter++;
	LeaveCriticalSection(&g_csAppCommonMutex);

	return TRUE;
}

BOOL DestroyRes(void)
{
	// �ȴ������߳��˳�
	EnterCriticalSection(&g_csAppCommonMutex);
	while (g_i32ThreadCounter == 0)
		Sleep(20);
	LeaveCriticalSection(&g_csAppCommonMutex);

	if (g_hNamedPipeToOutputWnd != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hNamedPipeToOutputWnd);
		g_hNamedPipeToOutputWnd = INVALID_HANDLE_VALUE;
	}

	if (g_hCanCreateProcessEvt != INVALID_HANDLE_VALUE && g_hCanCreateProcessEvt != NULL)
	{
		CloseHandle(g_hCanCreateProcessEvt);
		g_hCanCreateProcessEvt = INVALID_HANDLE_VALUE;
	}

	// �����ٽ���
	DeleteCriticalSection(&g_csAppLogMutex);
	DeleteCriticalSection(&g_csAppCommonMutex);
	return TRUE;
}

BOOL WriteAppLog(char *szLog)
{
	char szWriteLog[1024] = "";
	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	sprintf(szWriteLog, "[%04d/%02d/%02d %02d:%02d:%02d:%03d][TID%u] %s",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentThreadId(), szLog);
	EnterCriticalSection(&g_csAppLogMutex);
	FILE *pFile = fopen(g_szAppLogName, "a+");
	if (pFile != NULL)
	{
		fwrite(szWriteLog, sizeof(char), strlen(szWriteLog), pFile);
		fclose(pFile);
	}
	LeaveCriticalSection(&g_csAppLogMutex);
	return TRUE;
}

BOOL SendMsgToOutputWnd(char *szBuf, BOOL bShowTime)
{
	if (g_hNamedPipeToOutputWnd == INVALID_HANDLE_VALUE)
	{
		// ������ܵ�
		char szPipeName[256] = "";
		GetPrivateProfileString("NamedPipe", "OutputWnd", "ERROR", szPipeName, sizeof(szPipeName), g_szAppCfgName);
		if (strcmp(szPipeName, "ERROR") == 0)
			return FALSE;
		g_hNamedPipeToOutputWnd = CreateFile(szPipeName, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (g_hNamedPipeToOutputWnd == INVALID_HANDLE_VALUE)
			return FALSE;
		//if (WaitNamedPipe(szPipeName, 20000) == FALSE)
		//	return FALSE;
		DWORD dwMode = PIPE_READMODE_MESSAGE;
		SetNamedPipeHandleState(g_hNamedPipeToOutputWnd, &dwMode, NULL, NULL);
	}

	DWORD dwWriteSize = 0;
	char szShowTime[128] = "";
	SYSTEMTIME st;
	memset(&st, 0, sizeof(&st));
	GetSystemTime(&st);
	sprintf(szShowTime, "[%04d-%02d-%02d %02d:%02d:%02d:%03d] ",
		st.wYear, st.wMonth, st.wDay, st.wHour+8, st.wMinute, st.wSecond, st.wMilliseconds);
	char szSendBuf[SENDBUFSIZE] = "";
	if (bShowTime == TRUE)
		strcpy(szSendBuf, szShowTime);;
	strcat(szSendBuf, szBuf);
	DWORD dwSendSize = strlen(szSendBuf);
	BOOL bResult = WriteFile(g_hNamedPipeToOutputWnd, szSendBuf, dwSendSize, &dwWriteSize, NULL);

	if (bResult == FALSE)
	{
		CloseHandle(g_hNamedPipeToOutputWnd);
		g_hNamedPipeToOutputWnd = INVALID_HANDLE_VALUE;
	}
	return bResult;
}

BOOL ExitProcessTree(BOOL bIsExitItself, DWORD dwThisProcessID)
{
	BOOL bRet = FALSE;
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};

	SetLastError(0);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap)
	{
		DWORD dwLastError = GetLastError();
		return FALSE;
	}

	if (0 == dwThisProcessID)
		dwThisProcessID = GetCurrentProcessId();
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hProcessSnap, &pe32))
	{
		do 
		{
			if (pe32.th32ParentProcessID == dwThisProcessID)
			{
				// Ϊ�ӽ���
				ExitProcessTree(TRUE, pe32.th32ProcessID);
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}

	if (TRUE == bIsExitItself && -1 != dwThisProcessID)
	{
		// ɱ���߳�
		HANDLE hKillProcess = NULL;
		hKillProcess = OpenProcess(PROCESS_TERMINATE, TRUE, dwThisProcessID);
		if (hKillProcess != NULL && TerminateProcess(hKillProcess, 1))
		{
			if (CloseHandle(hKillProcess))
			{
				bRet = TRUE;
			}
		}		
	}

	if (TRUE == bRet)
		bRet = CloseHandle(hProcessSnap);
	else
		CloseHandle(hProcessSnap);
	if (FALSE == bRet)
	{
		// ������־���
		char szOutput[256] = "";
		sprintf(szOutput, "�رս��� %d ʧ�ܣ�������Ϊ%d", dwThisProcessID, GetLastError());
		MessageBox(NULL, szOutput, "Error!", MB_OK);
	}
	return bRet;
}

BOOL GetFileNameAndPathByFullName(char __out *pFileName, char __out *pFilePath, char __in *pFileFullName)
{
	CHAR *pFindPos = NULL;
	CHAR *pLastFindPos = NULL;
	CHAR szTemp[512] = "";
	CHAR szFindChar[2] = "";
	strcpy(szTemp, pFileFullName);

	do
	{
		pFindPos = strstr(szTemp, "/");
		if (pFindPos != NULL)
			*pFindPos = '\\';
	} while(pFindPos != NULL);

	pFindPos = szTemp;
	do 
	{
		pFindPos = strstr(pFindPos, "\\");
			if (pFindPos != NULL)
			{
				pLastFindPos = pFindPos;
				pFindPos++;
			}
	} while (pFindPos != NULL);

	memcpy(pFileName, pLastFindPos+1, strlen(pLastFindPos)-1);
	*pLastFindPos = 0;
	strcpy(pFilePath, szTemp);
	return TRUE;
}