#pragma once

#include <process.h>
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "Psapi.h"
#pragma comment(lib, "Psapi.lib")
#include "CrashRpt.h"
#pragma comment(lib, "..\\..\\library\\CrashRpt.lib")
#include <d3d9.h>
#ifdef WIN32
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3d9.lib")
#else
#pragma comment(lib, "\\x64\\d3d9.lib")
#pragma comment(lib, "\\x64\\d3d9.lib")
#endif


void ShowErrorMessageA(char *szContent);
void ShowErrorMessageW(wchar_t *wsContent);
#ifdef UNICODE
#define ShowErrorMessage ShowErrorMessageW
#else
#define ShowErrorMessage ShowErrorMessageA
#endif

#pragma warning(disable: 4996)