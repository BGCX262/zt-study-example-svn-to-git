#pragma once
/************************************
	功能：自动编译工具
	作者：张探
************************************/


/*包含头文件*/
#include "ztCommon.h"


/*函数声明*/
void MainMsgProc(void);
BOOL WriteRunningLog(BOOL bIsWriteFile, wchar_t *wsFormat, ...);
BOOL WINAPI CrashProc(PEXCEPTION_POINTERS Exception);
BOOL SetWndFont(HWND hWnd, wchar_t *pName, INT32 i32Weight, INT32 i32Height);
LRESULT CALLBACK MainWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SubWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL SetCurrentTaskText(wchar_t *pText);					// 设置当前任务显示标签
BOOL SetCurrentTaskProgress(INT32 i32Progress);				// 设置当前任务进度
BOOL SetTotalTaskProgress(INT32 i32Progress);				// 设置总任务进度
BOOL ShowLoadScriptWnd(BOOL bIsShow);
unsigned __stdcall RunTaskThreadFunc(void *pParam);			// 控制线程
unsigned __stdcall RunControlThreadFunc(void *pParam);		// 任务线程
BOOL ShowLogControlWnd(void);
BOOL HideLogControlWnd(void);
BOOL CountTaskRunTime(void);