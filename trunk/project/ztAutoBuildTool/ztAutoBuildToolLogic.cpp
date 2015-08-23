/*包含头文件*/
#include "ztAutoBuildTool.h"


/*全局变量定义*/
ETASKSTATUS g_eCurrentTaskStatus = ETASKSTATUS_ERROR;
UINT32 g_ui32TaskBeginTime = 0;
UINT32 g_ui32TaskPauseTime = 0;


/*函数声明*/


/*函数实现*/
unsigned __stdcall RunTaskThreadFunc(void *pParam)
{
	while (TRUE)
	{
		if (g_bIsMainThreadExit == TRUE)
			break;

		Sleep(1);
	}
	return 0;
}

unsigned __stdcall RunControlThreadFunc(void *pParam)
{
	INT32 i32Temp = 0;
	while (TRUE)
	{
		if (g_bIsMainThreadExit == TRUE)
			break;

		CountTaskRunTime();
		Sleep(5);
	}
	return 0;
}

BOOL ShowLoadScriptWnd(BOOL bIsShow)
{
	if (bIsShow == TRUE)
	{
		RECT rc;
		memset(&rc, 0, sizeof(rc));
		GetWindowRect(g_hThisWndHandle, &rc);
		INT32 i32WndPosX = rc.left + (rc.right-rc.left-300)/2;
		INT32 i32WndPosY = rc.top + (rc.bottom-rc.top-150)/2 - 30;
		SetWindowPos(g_struSubWnd[10].m_hWnd, HWND_TOP, i32WndPosX, i32WndPosY, 0, 0, SWP_NOSIZE);
	}	

	ShowWindow(g_struSubWnd[10].m_hWnd, bIsShow);
	UpdateWindow(g_struSubWnd[10].m_hWnd);
	if (bIsShow == FALSE)
		SetFocus(g_hThisWndHandle);
	return TRUE;
}

BOOL SetCurrentTaskText(wchar_t *pText)
{
	return SetWindowText(g_struSubWnd[11].m_hWnd, pText);
}

BOOL SetCurrentTaskProgress(INT32 i32Progress)
{
	if (i32Progress < 0)
		i32Progress = 0;
	if (i32Progress > 100)
		i32Progress = 100;
	wchar_t wsShow[16] = L"";
	wsprintf(wsShow, L"%d%%", i32Progress);
	SetWindowText(g_struSubWnd[12].m_hWnd, wsShow);
	SendMessage(g_struSubWnd[6].m_hWnd, PBM_SETPOS, (WPARAM)i32Progress, 0);
	return TRUE;
}

BOOL SetTotalTaskProgress(INT32 i32Progress)
{
	if (i32Progress < 0)
		i32Progress = 0;
	if (i32Progress > 100)
		i32Progress = 100;
	wchar_t wsShow[16] = L"";
	wsprintf(wsShow, L"%d%%", i32Progress);
	SetWindowText(g_struSubWnd[16].m_hWnd, wsShow);
	SendMessage(g_struSubWnd[5].m_hWnd, PBM_SETPOS, (WPARAM)i32Progress, 0);
	return TRUE;
}

BOOL CountTaskRunTime(void)
{
	BOOL bIsUpdate = FALSE;
	UINT32 ui32CountTime = 0;
	static UINT32 ui32ShowTime = 0;
	EnterCriticalSection(&g_csMainThreadMutex);
	switch (g_eCurrentTaskStatus)
	{
	case ETASKSTATUS_ERROR:
		bIsUpdate = FALSE;
		break;

	case ETASKSTATUS_RUN:
		bIsUpdate = TRUE;
		break;

	case ETASKSTATUS_STOP:
		bIsUpdate = FALSE;
		SetWindowText(g_struSubWnd[14].m_hWnd, L"00:00:00");
		break;

	case ETASKSTATUS_PAUSE:
		g_ui32TaskBeginTime += (GetTickCount() - g_ui32TaskPauseTime);
		bIsUpdate = FALSE;
		break;

	default:
		LeaveCriticalSection(&g_csMainThreadMutex);
		return FALSE;
	}
	if (bIsUpdate == TRUE)
		ui32CountTime = (GetTickCount() - g_ui32TaskBeginTime) / 1000;
	LeaveCriticalSection(&g_csMainThreadMutex);
	if (bIsUpdate == TRUE && ui32ShowTime != ui32CountTime)
	{
		ui32ShowTime = ui32CountTime;
		wchar_t wsShowTime[64] = L"";
		wsprintf(wsShowTime, L"%02d:%02d:%02d", ui32ShowTime/(3600), ui32ShowTime%(3600)/60, ui32ShowTime%(3600)%60);
		SetWindowText(g_struSubWnd[14].m_hWnd, wsShowTime);
	}
	return TRUE;
}

BOOL SetCurrentTaskStatus(ETASKSTATUS eStatus)
{
	EnterCriticalSection(&g_csMainThreadMutex);
	if (g_eCurrentTaskStatus == eStatus) 
	{
		LeaveCriticalSection(&g_csMainThreadMutex);
		return FALSE;
	}
	g_eCurrentTaskStatus = eStatus;
	LeaveCriticalSection(&g_csMainThreadMutex);

	switch (eStatus)
	{
	case ETASKSTATUS_ERROR:
		break;

	case ETASKSTATUS_RUN:
		break;

	case ETASKSTATUS_STOP:
		break;

	case ETASKSTATUS_PAUSE:
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

BOOL ShowLogControlWnd(void)
{
	// 获取显示坐标
	RECT rcMain;
	POINT pointCursor ;
	INT32 i32ShowPosX = 0;
	INT32 i32ShowPosY = 0;
	if (GetCursorPos(&pointCursor) == FALSE) return FALSE;
	if (GetWindowRect(g_hThisWndHandle, &rcMain) == FALSE) return FALSE;
	i32ShowPosX = pointCursor.x - rcMain.left-3;
	i32ShowPosY = pointCursor.y - rcMain.top-22;
	if (i32ShowPosX + 100 >= g_i32MainWndWidth)
		i32ShowPosX = i32ShowPosX-100;
	if (i32ShowPosY + 50 >= g_i32MainWndWidth)
		i32ShowPosY = i32ShowPosY - 50;
	if (SetWindowPos(g_struSubWnd[17].m_hWnd, HWND_TOP, i32ShowPosX, i32ShowPosY, 0, 0, SWP_NOSIZE|SWP_NOZORDER) == FALSE) return FALSE;
	if (ShowWindow(g_struSubWnd[17].m_hWnd, 1) == FALSE) return FALSE;
	return UpdateWindow(g_struSubWnd[17].m_hWnd);
}

BOOL HideLogControlWnd(void)
{
	return ShowWindow(g_struSubWnd[17].m_hWnd, 0);
}