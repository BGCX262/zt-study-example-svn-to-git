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
	sprintf(szTemp, "�ɹ��޸�%u����ʧ��%u���������ļ�%u��������ʱ%.2fs!\n", 
		g_ui32successesCount, g_ui32FailCount, g_ui32NormalCount, (double)(stEndTime-stBeginTime) / 1000.0);
	WriteAppLog(szTemp);
	printf("���س����˳�!\n");
	getchar();
	return 0;
}

BOOL SearchSpecialFile(char *szSearchDir)
{
	// ����.htm��.html�ļ�
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
		if (strcmp(struFindFileData.cFileName, ".") == 0 || strcmp(struFindFileData.cFileName, "..") == 0) 	// ����.��..
			continue;
		if( struFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			char szNextDir[512] = "";
			sprintf(szNextDir, "%s%s\\", szSearchDir, struFindFileData.cFileName);
			SearchSpecialFile(szNextDir);																	// ������һ��Ŀ¼
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
	// ��ȡ�ļ������ʽ
	FILE *pFile = fopen(szFileFullName, "r");
	if (pFile == NULL)
	{
		char szLog[512] = "";
		sprintf(szLog, "��ֻ����ʽ��%sʧ�ܣ�\n", szFileFullName);
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

	// ��ȡ�ļ�����
	fseek(pFile, 0, SEEK_END);
	fpos_t i64Size = 0;
	fgetpos(pFile, &i64Size);							// ��ȡ�ļ���С
	void *pBuffer = malloc(i64Size+2);					// �ڴ�����
	memset(pBuffer, 0, i64Size+2);
	fseek(pFile, 0, SEEK_SET);							// �ļ�ָ�뷵���ļ�ͷ
	if (bIsMutitByteChar == TRUE)
		fread(pBuffer, sizeof(char), i64Size, pFile);
	else
		fread(pBuffer, sizeof(wchar_t), (i64Size)/(sizeof(wchar_t)), pFile);
	fclose(pFile);

	// �ļ���Ч���ж�
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
		sprintf(szLog, "�ļ�%s����������html��ʽ�ļ���\n", szFileFullName);
		WriteAppLog(szLog);
		free(pBuffer);
		return 0;
	}

	// ���������ж�
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
		if (i32EndLen < 15)			// 15Ϊ����ֵ�����¸���5���С�
		{
			// �����ļ�
			char szLog[512] = "";
			sprintf(szLog, "ɨ��%s ����!\n", szFileFullName);
			WriteAppLog(szLog);
			free(pBuffer);
			return 2;
		}
	}

	// ɾ������������
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
		sprintf(szLog, "�Զ�д��ʽ��%sʧ�ܣ�\n", szFileFullName);
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
	sprintf(szLog, "�ļ�%s�޸����!\n", szFileName);
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