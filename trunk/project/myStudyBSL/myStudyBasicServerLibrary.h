/**********************
�����������������
���ã��ṩ���������Ͷ��壬�������ļ��������̡߳��ܵ����¼�����غ���
���ߣ���̽
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