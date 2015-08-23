#pragma once
#include "myStudyCommon.h"

static INT32 ProcCmdLine(LPSTR lpCmdLine);		// 处理命令行
static BOOL RegisterMyClass(void);				// 窗口类注册
static BOOL CreateMyWindow(INT32 cmdShow);		// 窗口创建
static BOOL DestroyMyWindow(void);				// 窗口销毁
LRESULT CALLBACK MainWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);		// 主窗口消息回调
LRESULT CALLBACK InputCtrlMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI CrashProc(PEXCEPTION_POINTERS Exception);

static void InitAppData(void);
static void ShowMessageBox(wchar_t *pCaption, wchar_t wsTextFormat, ...);			// 此函数未实现
void OnPressUpKey(HWND hWnd, LPARAM lParam);				// 上键按下
void OnPressDownKey(HWND hWnd, LPARAM lParam);				// 下键按下
void OnPressLeftKey(HWND hWnd, LPARAM lParam);				// 左键按下
void OnPressRightKey(HWND hWnd, LPARAM lParam);				// 右键按下

extern HWND g_hThisWndHandle;					// 本窗口句柄
extern HINSTANCE g_hThisInstance;				// 本实例句柄
extern wchar_t g_wsClassName[128];				// 窗口类名
extern WNDPROC g_lpfnInputWndBackup;			// 输入窗口原回调函数 