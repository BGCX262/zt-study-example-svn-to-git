#pragma once
#pragma warning(disable: 4244)
#pragma warning(disable: 4996)

#include <Windows.h>
#include <iostream>

BOOL SearchSpecialFile(char *szSearchDir);
BOOL GetFileFormat(char __out *szFormat, char __in *szFileName);
INT8 ClearVBScriptVirus(char __in *szFileFullName, char __in *szFileName);
BOOL GetCurrentProcessPath(char __out *szPaht);
BOOL WriteAppLog(char *szLog);