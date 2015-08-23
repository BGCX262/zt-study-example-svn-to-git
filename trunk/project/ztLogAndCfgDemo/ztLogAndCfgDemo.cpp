/*包含头文件及依赖库*/
#include "common.h"
#include "ztLogAndCfgDemo.h"


/*全局变量声明*/
HWND g_hFatherWnd = NULL;													// 调用方传如的窗口句柄
HWND g_hMainWnd = NULL;														// 主窗口句柄
HWND g_hLogWnd = NULL;														// Log窗口句柄
HWND g_hCfgWnd = NULL;														// 配置窗口
HWND g_hLineLossWnd = NULL;													// 线损窗口
HWND g_hLimitsWnd = NULL;													// 限制窗口
HINSTANCE g_hThisInstance = NULL;											// 实例句柄
wchar_t g_wsWndClsName[64] = L"";											// 主窗口类名
wchar_t g_wsThisCfgName[256] = L"";											// 配置文件名			
wchar_t g_wsThisCfgFullPath[256] = L"";										// 配置文件完整文件名(绝对路径)
wchar_t g_wsThisInstancePath[256] = L"";									// 本实例当前所在路劲
int g_iStatusInitShow = 0;													// 进程初始显示状态
HANDLE g_hLogicThread = INVALID_HANDLE_VALUE;								// 逻辑线程句柄
BOOL g_bIsMainThreadExit = FALSE;											// 主线程退出标志
vector<HFONT> g_vFontRes;
UINT32 g_ui32LogIndex = 0;
BOOL g_bIsSaveLogToFile = FALSE;


/*函数实现*/
/*int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 初始化程序数据
	if (InitAppData(hInstance, hPrevInstance, lpCmdLine, nCmdShow) == FALSE)
		return -6;
	
	// 注册窗口类
	if (RegisterDemoClass() ==  FALSE)
		return -1;					// 注册窗口类错误
	
	// 创建窗口
	if (CreateDemoWindow() == FALSE)
	{
		// 窗口创建失败后注销窗口类
		if (UnRegisterDemoClass() == FALSE)
			return -2;				// 创建窗口失败后，注销窗口类错误
		else
			return -3;				// 创建窗口错误
	}

	// 消息拾取
	MainMsgPeek();

	// 销毁窗口
	if (DestroyDemoWindow() == FALSE)
	{
		DWORD dwLastError = GetLastError();
		return -4;
	}

	// 注销窗口类
	if (UnRegisterDemoClass() == FALSE)
		return -5;					// 退出时注销窗口类错误
	return 0;
}*/

BOOL WINAPI DllMain(__in  HINSTANCE hinstDLL, __in  DWORD fdwReason, __in  LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_DETACH:
		break;

	case DLL_PROCESS_ATTACH:
		g_hThisInstance = hinstDLL;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}

BOOL __stdcall InitLogAndCfgWnd(HWND hFatherWnd)
{
//	MessageBox(NULL, L"DEBUG", L"DEBUG", MB_OK);

	g_vFontRes.clear();
	g_hFatherWnd = hFatherWnd;
	if (InitAppData(g_hThisInstance, NULL, "", SW_SHOW) == FALSE)
		return FALSE;
	if (RegisterDemoClass() ==  FALSE)
		return FALSE;
	return CreateDemoWindow();
}

BOOL __stdcall ShowCfgWnd(void)
{
	if (g_hFatherWnd == NULL || g_hCfgWnd == NULL)
		return FALSE;

	RECT rcCfgWnd;
	RECT rcFatherWnd;
	GetWindowRect(g_hCfgWnd, &rcCfgWnd);
	GetWindowRect(g_hFatherWnd, &rcFatherWnd);
	INT32 i32WndPosX = rcFatherWnd.left + ((rcFatherWnd.right-rcFatherWnd.left)-(rcCfgWnd.right-rcCfgWnd.left))/2;
	INT32 i32WndPosY = rcFatherWnd.top + ((rcFatherWnd.bottom-rcCfgWnd.top)-(rcCfgWnd.bottom-rcCfgWnd.top))/2;
	SetWindowPos(g_hCfgWnd, HWND_TOP, i32WndPosX, i32WndPosY, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	return ShowWindow(g_hCfgWnd, SW_SHOW);
}

BOOL __stdcall HideCfgWnd(void)
{
	return ShowWindow(g_hCfgWnd, SW_HIDE);
}

BOOL __stdcall ShowLineLossWnd(void)
{
	return ShowWindow(g_hLineLossWnd, SW_SHOW);
}

BOOL __stdcall HideLineLossWnd(void)
{
	return ShowWindow(g_hLineLossWnd, SW_HIDE);
}

BOOL __stdcall ShowLimitsWnd(void)
{
	return ShowWindow(g_hLimitsWnd, SW_SHOW);
}

BOOL __stdcall HideLimitsWnd(void)
{
	return ShowWindow(g_hLimitsWnd, SW_HIDE);
}

BOOL __stdcall DestroyLogAndCfgRes(void)
{
	g_bIsMainThreadExit = TRUE;
	if (DestroyDemoWindow() == FALSE)
		return FALSE;
	if (UnRegisterDemoClass() == FALSE)
		return FALSE;
	return TRUE;
}

BOOL __stdcall AddLogToWndW(wchar_t *wsLog, double dbTimeMS)
{
	BOOL bIsNeedScroll = FALSE;
	INT32 i32CurselPos = 0;
	INT32 i32LogCount = 0;
	i32CurselPos = SendMessage(g_hLogWnd, LB_GETCURSEL, NULL, NULL);
	i32LogCount = SendMessage(g_hLogWnd, LB_GETCOUNT, NULL, NULL);
	if (i32LogCount == i32CurselPos+1)
		bIsNeedScroll = TRUE;

	char szShowTime[64] = "";
	wchar_t wsShowTime[64] = L"";
	wchar_t wsShowLog[256] = L"";
	wchar_t wsShowLogTemp[128] = L"";
	if (wcslen(wsLog) > 66)
	{
		memcpy(wsShowLogTemp, wsLog, sizeof(wchar_t)*62);
		wsShowLogTemp[62+0] = '.';
		wsShowLogTemp[62+1] = '.';
		wsShowLogTemp[62+2] = '.';
		wsShowLogTemp[62+3] = 0;
	}
	else
		wcscpy(wsShowLogTemp, wsLog);
	if (dbTimeMS < 100.0)
		sprintf(szShowTime, "%.3lfms", dbTimeMS);
	else if (dbTimeMS < 1000.0)
		sprintf(szShowTime, "%.2lfms", dbTimeMS);
	else if (dbTimeMS < 10000.0)
		sprintf(szShowTime, "%.1lfms", dbTimeMS);
	else if (dbTimeMS < 100000.0)
		sprintf(szShowTime, "%.3lfs", dbTimeMS/1000.0);
	else if (dbTimeMS < 1000000.0)
		sprintf(szShowTime, "%.2lfs", dbTimeMS/1000.0);
	else if (dbTimeMS < 10000000.0)
		sprintf(szShowTime, "%.1lfs", dbTimeMS/1000.0);
	else
		sprintf(szShowTime, "%ds", (INT32)(dbTimeMS/1000));
	mbstowcs(wsShowTime, szShowTime, sizeof(szShowTime));
	wsprintf(wsShowLog, L"%06d  %-67s %-8s", ++g_ui32LogIndex%1000000, wsShowLogTemp, wsShowTime);
	SendMessage(g_hLogWnd, LB_ADDSTRING, 0, (LPARAM)wsShowLog);
	if (bIsNeedScroll == TRUE)
	{
		SetLastError(0);
		LRESULT ret = SendMessage(g_hLogWnd, LB_SETCURSEL, (WPARAM)(i32LogCount), NULL);
		DWORD dwLastError = GetLastError();
		int a = 0;
	}
	return TRUE;
}

BOOL __stdcall AddLogToWndA(char *szLog, double dbTimeMS)
{
	size_t stLen = strlen(szLog);
	wchar_t *wsLog = (wchar_t*)malloc(stLen);
	if (wsLog == NULL)
		return FALSE;
	mbstowcs(wsLog, szLog, stLen);
	BOOL bRet = AddLogToWndW(wsLog, dbTimeMS);
	free(wsLog);
	return bRet;
}

BOOL __stdcall ClearLogWnd(void)
{
	UINT32 ui32Count = (UINT32)SendMessage(g_hLogWnd, LB_GETSELCOUNT, 0, 0);
	for (UINT32 _i = 0; _i < ui32Count; _i++)
	{
		SendMessage(g_hLogWnd, LB_GETSELCOUNT, (WPARAM)_i, 0);
	}
	g_ui32LogIndex = 0;
	return TRUE;
}

BOOL __stdcall IsSaveLogToFile(BOOL bIsSave)
{
	g_bIsSaveLogToFile = bIsSave;
	return g_bIsSaveLogToFile;
}