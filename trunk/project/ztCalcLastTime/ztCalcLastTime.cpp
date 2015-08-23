#include "ztCalcLastTime.h"

int main(int argc, char* argv[])
{
	int a = 0xBFFF0015;

	LARGE_INTEGER llCpuFreq;
	memset(&llCpuFreq, 0, sizeof(LARGE_INTEGER));
	HANDLE hEndEvt = CreateEvent(NULL, FALSE, FALSE, "COUNTTIME_END");
	HANDLE hBeginEvt = CreateEvent(NULL, FALSE, FALSE, "COUNTTIME_BEGIN");
	QueryPerformanceFrequency(&llCpuFreq);
	
	if (hEndEvt == NULL || hBeginEvt == NULL)
	{
		printf("Create event failed!!!\n");
	}

	while(TRUE)
	{
		//static UINT32 ui32SleepTime = 1;
		//static UINT8 ui8Times = 0;
		//if (ui8Times++ >= 3)
		//{
		//	ui8Times = 0;
		//	if (ui32SleepTime < 10)
		//		ui32SleepTime++;
		//	else if (ui32SleepTime < 100)
		//		ui32SleepTime += 10;
		//	else if (ui32SleepTime < 1000)
		//		ui32SleepTime += 100;
		//	else if (ui32SleepTime < 10000)
		//		ui32SleepTime += 1000;
		//	else
		//		ui32SleepTime += 2000;
		//	continue;
		//}
		LARGE_INTEGER llEndCount;
		LARGE_INTEGER llBeginCount;
		memset(&llEndCount, 0, sizeof(LARGE_INTEGER));
		memset(&llBeginCount, 0, sizeof(LARGE_INTEGER));
		WaitForSingleObject(hBeginEvt, INFINITE);
		if (QueryPerformanceCounter(&llBeginCount) == FALSE)
		{
			printf("QueryPerformanceCounter return false! Last error is %u! llBeginCount\n", GetLastError());
			continue;
		}

		//Sleep(ui32SleepTime);

		WaitForSingleObject(hEndEvt, INFINITE);
		if (QueryPerformanceCounter(&llEndCount) == FALSE)
		{
			printf("QueryPerformanceCounter return false! Last error is %u! llEndCount\n", GetLastError());
			continue;
		}

		double dbMidTime1 = 0.0;
		double dbMidTime2 = 0.0;
		double dbLastTime = static_cast<double>((static_cast<double>(llEndCount.QuadPart-llBeginCount.QuadPart)*1000.0) / static_cast<double>(llCpuFreq.QuadPart));
		char szLog[256] = "";
		ShowCurTime();
	//	sprintf(szLog, "  Sleep(%u)  %.3fms\n", ui32SleepTime, dbLastTime);
		sprintf(szLog, "  %.3fms\n", dbLastTime);
		printf(szLog);
		WriteLogFile(szLog);

		//if (ui32SleepTime >= 10000)
		//	Sleep(INFINITE);
	}
}

BOOL WriteLogFile(char __in *szLog, char __in *szLogName, char __in szLogPath[])
{
	// 获取本地时间
	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);

	// 创建多级目录
	if (CreateMultiDirectory(szLogPath) == FALSE)
		return FALSE;

	char szLogFileName[256] = "";
	if (szLogName == NULL)
	{
		char szThisModule[256] = "";
		GetModuleBaseName(GetCurrentProcess(), NULL, szThisModule, sizeof(szThisModule));
		szLogName = szThisModule;
	}
	sprintf(szLogFileName, "%s%s_%04d%02d%02d.log", szLogPath, szLogName, st.wYear, st.wMonth, st.wDay);
	FILE *pFile = fopen(szLogFileName, "a+");
	if (pFile == NULL)
		return FALSE;
	
	UINT32 ui32BufferSize = strlen(szLog) + 30;
	char *pWriteLog = (char*)malloc(ui32BufferSize);
	memset((void*)pWriteLog, 0, ui32BufferSize);
	sprintf(pWriteLog, "[%02d:%02d:%02d.%03d] %s",st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, szLog);
	fwrite(pWriteLog, sizeof(char), strlen(pWriteLog), pFile);
	free(pWriteLog);
	fclose(pFile);
	return TRUE;
}

// 创建目录，支持创建多级目录
BOOL CreateMultiDirectory(char __in *szPath)
{
	char *szFindPos = NULL;
	char szTempPath[256] = "";
	char szTempPathBak[256] = "";
	static UINT32 ui32CutLen = 0;
	ui32CutLen = 0;		// 必须清零

	// 统一路径格式
	if (*szPath == '\\' || *szPath == '/')
		sprintf(szTempPath, ".%s", szPath);
	else
		strcpy(szTempPath, szPath);
	UINT32 ui32Len = strlen(szTempPath);
	for (UINT32 _i = 0; _i < ui32Len; _i++)
	{
		if (szTempPath[_i] == '/')
			szTempPath[_i] = '\\';
	}
	strcpy(szTempPathBak, szTempPath);

	// 路径分析
	szFindPos = strstr(szTempPath, "\\");
	while (szFindPos++ != NULL)
	{
		char szCutPath[256] = "";
		ui32CutLen += (szFindPos - szTempPath);			// 计算截取长度
		memcpy(szCutPath, szTempPathBak, ui32CutLen);
		if (szCutPath[ui32CutLen-2] == '.' && szCutPath[ui32CutLen-1] == '\\')
		{
			// 过滤 . ..
		}
		else if (szCutPath[ui32CutLen-2] != ':')		// 非根路径
		{
			// 创建文件夹
			if (szCutPath[ui32CutLen-1] == '\\')
				szCutPath[ui32CutLen-1] = 0;	// 去掉结尾'\\'
			if (IsFolderExist(szCutPath) == FALSE)
			{
				if (CreateDirectory(szCutPath, NULL) == FALSE)
					return FALSE;
			}
		}
		strcpy(szTempPath, szFindPos);
		szFindPos = strstr(szTempPath, "\\");
	}
	return TRUE;
}

// 删除目录，支持删除非空目录
BOOL DeleteMultiDirectory(char __in *szPath)
{
	HANDLE pNextFile;
	WIN32_FIND_DATA win32FindFile;
	char strFullPath[256] = "";
	char strFullFileName[256] = "";

	strcpy(strFullPath, szPath);
	sprintf(strFullFileName, "%s/*.*", szPath);

	pNextFile = FindFirstFile(strFullFileName, &win32FindFile);

	while( FindNextFile( pNextFile, &win32FindFile ) )
	{
		if (strcmp(win32FindFile.cFileName, ".") == 0 || strcmp(win32FindFile.cFileName, "..") == 0)			//过滤..和.
			continue;

		if (win32FindFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			sprintf(strFullPath, "%s/%s", szPath, win32FindFile.cFileName);
			DeleteMultiDirectory(strFullPath);
		}
		else
		{
			memset(strFullFileName, 0, sizeof(strFullFileName));
			sprintf( strFullFileName, "%s/%s", szPath, win32FindFile.cFileName );
			DeleteFile(strFullFileName);
		}
	}

	FindClose(pNextFile);
	return RemoveDirectory(szPath);
}

// 文件夹是否存在
BOOL IsFolderExist(char *szFolderName)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(szFolderName, &fd);
	BOOL bRetValue = (hFind != INVALID_HANDLE_VALUE) ? TRUE : FALSE;
	FindClose(hFind);
	return bRetValue;
}

// 控制台输出当前时间
BOOL ShowCurTime(void)
{
	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetSystemTime(&st);
	printf("%02d:%02d:%02d.%03d", st.wHour+8, st.wMinute, st.wSecond, st.wMilliseconds);
	return TRUE;
}