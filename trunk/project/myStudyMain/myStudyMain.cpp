#include <iostream>
#include <sys/stat.h>
#include "myStudyMain.h"
using namespace std;

HWND g_hThisWndHandle = NULL;					// 本窗口句柄
HINSTANCE g_hThisInstance = NULL;				// 本实例句柄
wchar_t g_wsClassName[128] = L"";				// 窗口类名
WNDPROC g_lpfnInputWndBackup = NULL;			// 输入窗口原回调函数 

INT32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	InitializeCrashRptEx((CRASHRPTPROC)CrashProc);

	MEMORYSTATUSEX struMem;
	memset(&struMem, 0, sizeof(MEMORYSTATUSEX));
	struMem.dwLength = sizeof(MEMORYSTATUSEX);
	if (GlobalMemoryStatusEx(&struMem) == FALSE)
	{
		DWORD dwError = GetLastError();
		INT32 i = 0;
	}
	ProcCmdLine(lpCmdLine);
	InitAppData();
	if (RegisterMyClass() && CreateMyWindow(nShowCmd)) 
	{  
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) 
		{  
			TranslateMessage(&msg);  
			DispatchMessage(&msg);  
		} 
		DestroyMyWindow();
		return static_cast<int>(msg.wParam);
	} 
	else 
	{
		DestroyMyWindow();
		wchar_t wsOutput[128] = L"";
		wsprintf(wsOutput, L"Create window failed! Error code is %d", GetLastError());
		MessageBox(NULL, wsOutput, L"Error!", MB_OK);
	} 
	return 0;
}

static BOOL RegisterMyClass() 
{  
	WNDCLASSEX  wce;
	memset(&wce, 0, sizeof(WNDCLASSEX));
	wce.cbSize = sizeof(wce);  
	wce.style = CS_VREDRAW | CS_HREDRAW;  
	wce.lpfnWndProc = &MainWndMsgProc;				//指明回调函数  
	wce.hInstance = GetModuleHandle(NULL);  
	wce.hIcon = LoadIcon(0, MAKEINTRESOURCE(IDI_WINLOGO));  
	wce.hCursor = LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));  
	wce.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	//wce.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE);  
	wce.lpszClassName = g_wsClassName;				//独一无二的类名  
	wce.hIconSm = wce.hIcon;  
	return 0 != RegisterClassEx(&wce);  
} 

static BOOL CreateMyWindow(INT32 cmdShow) 
{  
	DWORD dwWndStyle = 0;
	DWORD dwWndStyleEx = 0;
	INT32 i32SrnWidth = GetSystemMetrics(SM_CXSCREEN);
	INT32 i32SrnHeight = GetSystemMetrics(SM_CYSCREEN);
	INT32 i32WndWidth = GetPrivateProfileInt(L"wnd", L"width", DEFAULTWNDWEITH, L".\\config.ini");
	INT32 i32WndHeight = GetPrivateProfileInt(L"wnd", L"height", DEFAULTWNDHEIGHT, L".\\config.ini");
	INT32 i32WndPosX = (i32SrnWidth - i32WndWidth) / 2 > 0 ? (i32SrnWidth - i32WndWidth) / 2 : 0;
	INT32 i32WndPosY = (i32SrnHeight - i32WndHeight) / 2 > 0 ? (i32SrnHeight - i32WndHeight) / 2 : 0;
	if (GetPrivateProfileInt(L"wnd", L"border", DEFAULTWNDWEITH, L".\\config.ini"))
		dwWndStyle = WS_OVERLAPPEDWINDOW;
	else
		dwWndStyle = WS_POPUP;
	if (GetPrivateProfileInt(L"wnd", L"istop", DEFAULTWNDWEITH, L".\\config.ini"))
		dwWndStyleEx = WS_EX_TOPMOST;

	// 窗口长或宽任意为0则全屏显示
	if (i32WndWidth == 0  || i32WndHeight == 0)
	{
		i32WndHeight = i32SrnHeight;
		i32WndWidth = i32SrnWidth;
		i32WndPosX = 0;
		i32WndPosY = 0;
		dwWndStyle = WS_POPUP;
	}

	g_hThisWndHandle = CreateWindowEx(
		dwWndStyleEx, 
		g_wsClassName, 
		L"Demo", 
		dwWndStyle,  
		i32WndPosX, 
		i32WndPosY, 
		i32WndWidth, 
		i32WndHeight,  
		NULL, 
		NULL, 
		GetModuleHandle(NULL), 
		NULL);  

	if (NULL == g_hThisWndHandle) 
		return FALSE;
	else
	{
		ShowWindow(g_hThisWndHandle, cmdShow);
		UpdateWindow(g_hThisWndHandle); 
	}

	HWND hWnd = CreateWindowEx(
		NULL, 
		L"EDIT", 
		L"InputURL", 
		WS_POPUP, 
		20 + i32WndPosX, 
		20 + i32WndPosY, 
		i32WndWidth - 40, 
		20, 
		g_hThisWndHandle, 
		NULL, 
		GetModuleHandle(NULL), 
		NULL);
	if (hWnd == NULL)
		return FALSE;
	else
	{
		g_lpfnInputWndBackup = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)InputCtrlMsgProc);
		if (g_lpfnInputWndBackup == NULL)
		{
			DWORD dwLastError = GetLastError();
			return FALSE;
		}

		ShowWindow(hWnd, cmdShow);
		UpdateWindow(hWnd);
	} 

	return TRUE;
}

static BOOL DestroyMyWindow(void)
{
	if (NULL != g_hThisWndHandle)
	{
		DestroyWindow(g_hThisWndHandle);
		g_hThisWndHandle = NULL;
	}

	return UnregisterClass(g_wsClassName, g_hThisInstance);
}

static void InitAppData(void)
{
	g_hThisWndHandle = NULL;
	wcscpy(g_wsClassName, L"myFristClass");
}

static void ShowMessageBox(wchar_t *pCaption, wchar_t wsTextFormat, ...)
{
	// 未实现
}

void OnPressUpKey(HWND hWnd, LPARAM lParam)
{
	RECT rc;
	memset(&rc, 0, sizeof(RECT));
	GetWindowRect(hWnd, &rc);
	INT32 i32PosY = rc.top - YMOVEWNDSTEP > 0 ? rc.top - YMOVEWNDSTEP : 0;
	SetWindowPos(hWnd, HWND_TOP, rc.left, i32PosY, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
}

void OnPressDownKey(HWND hWnd, LPARAM lParam)
{
	RECT rc;
	memset(&rc, 0, sizeof(RECT));
	GetWindowRect(hWnd, &rc);
	INT32 i32PosY = rc.bottom + YMOVEWNDSTEP < GetSystemMetrics(SM_CYSCREEN) ? rc.top + YMOVEWNDSTEP : GetSystemMetrics(SM_CYSCREEN) - (rc.bottom- rc.top);
	SetWindowPos(hWnd, HWND_TOP, rc.left, i32PosY, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
}

void OnPressLeftKey(HWND hWnd, LPARAM lParam)
{
	RECT rc;
	memset(&rc, 0, sizeof(RECT));
	GetWindowRect(hWnd, &rc);
	INT32 i32PosX = rc.left - XMOVEWNDSTEP > 0 ? rc.left - XMOVEWNDSTEP : 0;
	SetWindowPos(hWnd, HWND_TOP, i32PosX, rc.top, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
}

void OnPressRightKey(HWND hWnd, LPARAM lParam)
{
	RECT rc;
	memset(&rc, 0, sizeof(RECT));
	GetWindowRect(hWnd, &rc);
	INT32 i32PosX = rc.right + XMOVEWNDSTEP < GetSystemMetrics(SM_CXSCREEN) ? rc.left + XMOVEWNDSTEP : GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left);
	SetWindowPos(hWnd, HWND_TOP, i32PosX, rc.top, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
}

static INT32 ProcCmdLine(LPSTR lpCmdLine)
{
	return 1;
}