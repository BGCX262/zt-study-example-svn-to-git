#include "ztClearVBSVirus.h"


UINT32 g_ui32successesCount = 0;
UINT32 g_ui32FailCount = 0;
UINT32 g_ui32NormalCount = 0;

int main(int argc, char *argv[])
{
	char szTemp[512] = "";
	size_t stBeginTime = GetTickCount();
	size_t stEndTime = 0;
	GetCurrentProcessPath(szTemp);
	SearchSpecialFile(szTemp);
	stEndTime = GetTickCount();
	sprintf(szTemp, "成功修复%u个，失败%u个，正常文件%u个，共耗时%.2fs!\n", 
		g_ui32successesCount, g_ui32FailCount, g_ui32NormalCount, (double)(stEndTime-stBeginTime) / 1000.0);
	WriteAppLog(szTemp);
	printf("按回车键退出!\n");
	getchar();
	return 0;
}

BOOL SearchSpecialFile(char *szSearchDir)
{
	// 搜索.htm和.html文件
	char szSearchFileName[512] = "";
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA struFindFileData;
	sprintf(szSearchFileName, "%s*.*", szSearchDir);
	memset(&struFindFileData, 0, sizeof(WIN32_FIND_DATA));
	hFind = FindFirstFile(szSearchFileName, &struFindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		char szInput[128] = "";
		printf("No find!\n");
		return FALSE;
	}
	while (FindNextFile(hFind, &struFindFileData))
	{
		if (strcmp(struFindFileData.cFileName, ".") == 0 || strcmp(struFindFileData.cFileName, "..") == 0) 	// 过滤.和..
			continue;
		if( struFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			char szNextDir[512] = "";
			sprintf(szNextDir, "%s%s\\", szSearchDir, struFindFileData.cFileName);
			SearchSpecialFile(szNextDir);																	// 搜索下一级目录
		}

		char szFileFormat[128] = "";
		GetFileFormat(szFileFormat, struFindFileData.cFileName);
		if (strcmp(szFileFormat, "htm") == 0 || strcmp(szFileFormat, "html") == 0 || strcmp(szFileFormat, "inf") == 0 || strcmp(szFileFormat, "lnk") == 0)
		{
			char szFullName[512] = "";
			sprintf(szFullName, "%s%s", szSearchDir, struFindFileData.cFileName);
			INT8 i8Result = ClearVBScriptVirus(szFullName, struFindFileData.cFileName);
			if (i8Result == 1)
				g_ui32successesCount++;
			else if (i8Result == 0)
				g_ui32FailCount++;
			else
				g_ui32NormalCount++;
		}
		Sleep(1);
	}

	FindClose(hFind);
	return TRUE;
}

BOOL GetFileFormat(char __out *szFormat, char __in *szFileName)
{
	char szRevName[512] = "";
	char szRevFormat[128] = "";
	strcpy(szRevName, strrev(szFileName));
	strrev(szFileName);
	char* pFindDot = strstr(szRevName, ".");
	if (pFindDot == NULL) return FALSE;
	memcpy(szRevFormat, szRevName, pFindDot - szRevName);
	strcpy(szFormat, strrev(szRevFormat));
	return TRUE;
}

INT8 ClearVBScriptVirus(char __in *szFileFullName, char __in *szFileName)
{
	// 获取文件编码格式
	FILE *pFile = fopen(szFileFullName, "r");
	if (pFile == NULL)
	{
		char szLog[512] = "";
		sprintf(szLog, "以只读方式打开%s失败！\n", szFileFullName);
		WriteAppLog(szLog);
		return 0;
	}
	unsigned char szFileCode[3] = "";
	fread(szFileCode, 1, 2, pFile);
	BOOL bIsMutitByteChar = FALSE;
	if (szFileCode[0] == 0xff && szFileCode[1] == 0xfe)
		bIsMutitByteChar = FALSE;		// Unicode
	else
		bIsMutitByteChar = TRUE;		// Mutilcode

	// 读取文件内容
	fseek(pFile, 0, SEEK_END);
	fpos_t i64Size = 0;
	fgetpos(pFile, &i64Size);							// 获取文件大小
	void *pBuffer = malloc(i64Size+2);					// 内存申请
	memset(pBuffer, 0, i64Size+2);
	fseek(pFile, 0, SEEK_SET);							// 文件指针返回文件头
	if (bIsMutitByteChar == TRUE)
		fread(pBuffer, sizeof(char), i64Size, pFile);
	else
		fread(pBuffer, sizeof(wchar_t), (i64Size)/(sizeof(wchar_t)), pFile);
	fclose(pFile);

	// 文件有效性判断
	void *pFindPos = NULL;
	if (bIsMutitByteChar == TRUE)
		pFindPos = (void*)strstr((char*)pBuffer, "</html>");
	else
		pFindPos = (void*)wcsstr((wchar_t*)pBuffer, L"</html>");
	if (pFindPos == NULL)
	{
		if (bIsMutitByteChar == TRUE)
			pFindPos = (void*)strstr((char*)pBuffer, "</HTML>");
		else
			pFindPos = (void*)wcsstr((wchar_t*)pBuffer, L"</HTML>");
	}
	if (pFindPos == NULL)
	{
		char szLog[512] = "";
		sprintf(szLog, "文件%s不是正常的html格式文件！\n", szFileFullName);
		WriteAppLog(szLog);
		free(pBuffer);
		return 0;
	}

	// 病毒特征判断
	void *pVirus = NULL;
	if (bIsMutitByteChar == TRUE)
		pVirus = (void*)strstr((char*)pBuffer, "DropFileName = \"svchost.exe\"");
	else
		pVirus = (void*)wcsstr((wchar_t*)pBuffer, L"DropFileName = \"svchost.exe\"");
	if (pVirus == NULL)
	{
		INT32 i32EndLen = 0;
		if (bIsMutitByteChar == TRUE)
			i32EndLen = i64Size - ((char*)pFindPos - (char*)pBuffer);
		else
			i32EndLen = i64Size/2 - ((wchar_t*)pFindPos - (wchar_t*)pBuffer);
		if (i32EndLen < 15)			// 15为估计值，上下浮动5都行。
		{
			// 正常文件
			char szLog[512] = "";
			sprintf(szLog, "扫描%s 正常!\n", szFileFullName);
			WriteAppLog(szLog);
			free(pBuffer);
			return 2;
		}
	}

	// 删除非正常内容
	UINT32 ui32FileSize = 0;
	if (bIsMutitByteChar == TRUE)
		ui32FileSize = (char*)pFindPos - (char*)pBuffer + sizeof("</html>") - sizeof(char);
	else
		ui32FileSize = (char*)pFindPos - (char*)pBuffer + sizeof(L"</html>") - sizeof(wchar_t);
	void *pNewFileBuff = malloc(ui32FileSize+2);
	memset(pNewFileBuff, 0, ui32FileSize+2);
	memcpy(pNewFileBuff, pBuffer, ui32FileSize);

	if (bIsMutitByteChar == TRUE)
		pFile = fopen(szFileFullName, "w+");
	else
		pFile = fopen(szFileFullName, "wb+");
	if (pFile == NULL)
	{
		char szLog[512] = "";
		sprintf(szLog, "以读写方式打开%s失败！\n", szFileFullName);
		WriteAppLog(szLog);
		free(pBuffer);
		free(pNewFileBuff);
		return 0;
	}
	if (bIsMutitByteChar == TRUE)
		fwrite(pNewFileBuff, sizeof(char), strlen((char*)pNewFileBuff), pFile);
	else
		fwrite(pNewFileBuff, sizeof(wchar_t), wcslen((wchar_t*)pNewFileBuff), pFile);
	fclose(pFile);
	free(pNewFileBuff);
	free(pBuffer);

	char szLog[512] = "";
	sprintf(szLog, "文件%s修复完成!\n", szFileName);
	WriteAppLog(szLog);
	return 1;
}

BOOL GetCurrentProcessPath(char __out *szPaht)
{
	char *pFindPath = NULL;
	char szThisFileFullName[512] = "";
	char szThisFileFullReverseName[512] = "";
	GetModuleFileName(NULL, szThisFileFullName,  sizeof(szThisFileFullName));
	strcpy(szThisFileFullReverseName, strrev(szThisFileFullName));
	for (UINT16 _i = 0; _i < 512; _i++)
	{
		if (szThisFileFullReverseName[_i] == 0)
			break;
		if (szThisFileFullReverseName[_i] == '/')
			szThisFileFullReverseName[_i] = '\\';
	}
	pFindPath = strstr(szThisFileFullReverseName, "\\");
	strcpy(szPaht, strrev(pFindPath));
	return TRUE;
}

BOOL WriteAppLog(char *szLog)
{
	char szWriteLog[1024] = "";
	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	sprintf(szWriteLog, "[%04d/%02d/%02d %02d:%02d:%02d:%03d] %s",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, szLog);
	FILE *pFile = fopen(".\\run.log", "a+");
	if (pFile != NULL)
	{
		fwrite(szWriteLog, sizeof(char), strlen(szWriteLog), pFile);
		fclose(pFile);
	}
	printf(szWriteLog);
	return TRUE;
}