#include "ztHideFile.h"


BOOL SearchSpecialFile(char *szSearchDir)
{
	// 搜索.svn
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
		if (strcmp(struFindFileData.cFileName, ".") == 0 || strcmp(struFindFileData.cFileName, "..") == 0) 					//过滤.和..
			continue;
		if( struFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(struFindFileData.cFileName, ".svn") == 0)
			{
				if (!(struFindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
				{
					char szFileName[512] = "";
					sprintf(szFileName, "%s%s", szSearchDir, struFindFileData.cFileName);
					SetFileAttributes(szFileName, FILE_ATTRIBUTE_HIDDEN);
					printf(szFileName);
					printf("\n");
				}
			}
			else
			{
				char szNextDir[512] = "";
				sprintf(szNextDir, "%s%s\\", szSearchDir, struFindFileData.cFileName);
				SearchSpecialFile(szNextDir);
			}
		}
	}

	FindClose(hFind);
	return TRUE;
}

void Test1(void)
{
	// 获取当前进程路径
	char *pFindPath = NULL;
	char szThisFileFullName[512] = "";
	char szThisFileFullPath[512] = "";
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
	strcpy(szThisFileFullPath, strrev(pFindPath));

	SearchSpecialFile(szThisFileFullPath);

	char szTemp[128] = "";
	printf("Done it!!!\n");
	scanf("%s", szTemp);
}

#define  TRACE(s) OutputDebugString(s);
int Test2(void)
{
	return 0;
}

int main(int argc, char *argv[])
{
//	Test1();
	Test2();

	return 0;
}