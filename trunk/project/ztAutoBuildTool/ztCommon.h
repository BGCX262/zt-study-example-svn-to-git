#pragma once
#pragma warning(disable: 4996)


/*包含头文件和库文件*/
#include <Windows.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <stdio.h>
#include <process.h>
#include "Psapi.h"
#pragma comment(lib, "Psapi.lib")
#include "CrashRpt.h"
#pragma comment(lib, ".\\CrashRpt.lib")
#include "resource.h"


/*宏定义*/
#define MAX_WNDWIDGETS_COUNT	30		// 窗口小部件最大个数


/*自定义函数指针*/
typedef BOOL (__stdcall* WNDMSGCALLBACKPROC)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/*自定义类型及结构体、枚举*/
typedef enum eControlType
{
	ECONTROLTYPE_NULL = 0,
	ECONTROLTYPE_BUTTON,
	ECONTROLTYPE_LISTBOX,
	ECONTROLTYPE_LABLE,
}ECONTROLTYPE;

typedef struct struWndInfo
{
	HWND m_hWnd;						// 窗口句柄
	WNDMSGCALLBACKPROC m_lpfnWnd;		// 窗口回调函数
	ECONTROLTYPE m_eType;				// 窗口类型

	void clear()
	{
		m_hWnd = NULL;
		m_lpfnWnd = NULL;
		m_eType = ECONTROLTYPE_NULL;
	};
}STRUWNDINFO, *PSTRUWNDINFO;

typedef enum eTaskStatus
{
	ETASKSTATUS_ERROR = 0,
	ETASKSTATUS_RUN,
	ETASKSTATUS_PAUSE,
	ETASKSTATUS_STOP,
}ETASKSTATUS;


/*全局变量定义*/
extern INT32 g_i32MainWndWidth;
extern wchar_t g_wsWndClsName[256];
extern HWND g_hThisWndHandle;
extern STRUWNDINFO g_struSubWnd[MAX_WNDWIDGETS_COUNT];
extern BOOL g_bIsMainThreadExit;
extern CRITICAL_SECTION g_csMainThreadMutex;

extern UINT32 g_ui32TaskBeginTime;
extern UINT32 g_ui32TaskPauseTime;
extern ETASKSTATUS g_eCurrentTaskStatus;
