#pragma once
#pragma warning(disable: 4996)


/*����ͷ�ļ��Ϳ��ļ�*/
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


/*�궨��*/
#define MAX_WNDWIDGETS_COUNT	30		// ����С����������


/*�Զ��庯��ָ��*/
typedef BOOL (__stdcall* WNDMSGCALLBACKPROC)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/*�Զ������ͼ��ṹ�塢ö��*/
typedef enum eControlType
{
	ECONTROLTYPE_NULL = 0,
	ECONTROLTYPE_BUTTON,
	ECONTROLTYPE_LISTBOX,
	ECONTROLTYPE_LABLE,
}ECONTROLTYPE;

typedef struct struWndInfo
{
	HWND m_hWnd;						// ���ھ��
	WNDMSGCALLBACKPROC m_lpfnWnd;		// ���ڻص�����
	ECONTROLTYPE m_eType;				// ��������

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


/*ȫ�ֱ�������*/
extern INT32 g_i32MainWndWidth;
extern wchar_t g_wsWndClsName[256];
extern HWND g_hThisWndHandle;
extern STRUWNDINFO g_struSubWnd[MAX_WNDWIDGETS_COUNT];
extern BOOL g_bIsMainThreadExit;
extern CRITICAL_SECTION g_csMainThreadMutex;

extern UINT32 g_ui32TaskBeginTime;
extern UINT32 g_ui32TaskPauseTime;
extern ETASKSTATUS g_eCurrentTaskStatus;
