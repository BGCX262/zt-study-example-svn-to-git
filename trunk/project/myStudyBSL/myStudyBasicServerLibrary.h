/**********************
工程名：基本服务库
作用：提供基本的类型定义，及类似文件操作、线程、管道、事件等相关函数
作者：张探
**********************/
#pragma once
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#endif
#include "myStudyBSLCommon.h"

BOOL __stdcall GetProcessNameByPIDA(__out char* szProcessName, __in UINT32 dwPID);
BOOL __stdcall GetProcessNameByPIDW(__out wchar_t* wsProcessName, __in UINT32 dwPID);
#ifdef UNICODE
#define GetProcessNameByPID GetProcessNameByPIDW
#else
#define GetProcessNameByPID GetProcessNameByPIDA
#endif