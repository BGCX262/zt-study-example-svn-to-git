#pragma once
#include <windows.h>
#include <stdio.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

#pragma warning(disable: 4996)


BOOL ShowCurTime(void);
BOOL IsFolderExist(char *szFolderName);
BOOL CreateMultiDirectory(char __in *szPath);
BOOL DeleteMultiDirectory(char __in *szPath);
BOOL WriteLogFile(char __in *szLog, char __in *szLogName = NULL, char __in szLogPath[] = "d:\\logfile\\zhangtan\\");