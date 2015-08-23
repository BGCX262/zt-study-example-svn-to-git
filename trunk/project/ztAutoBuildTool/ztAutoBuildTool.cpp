/*包含头文件*/
#include "ztAutoBuildTool.h"
#include <string>
using namespace std;


/*全局变量定义*/
INT32 g_i32MainWndWidth = 0;
INT32 g_i32MainWndHeight = 0;
wchar_t g_wsWndClsName[256] = L"";
wchar_t g_wsMainWndTitle[128] = L"";
HWND g_hThisWndHandle = NULL;
BOOL g_bIsMainThreadExit = FALSE;
CRITICAL_SECTION g_csMainThreadMutex;
STRUWNDINFO g_struSubWnd[MAX_WNDWIDGETS_COUNT];
HANDLE g_hControlThread = INVALID_HANDLE_VALUE;
HANDLE g_hTaskThread = INVALID_HANDLE_VALUE;


/*函数声明*/
static BOOL InitAppData(HINSTANCE hInstance, LPSTR lpCmdLine);		// 初始化程序数据
static BOOL RegMyWndCls(void);										// 注册窗口类
static BOOL CreateMyWnd(int nShowCmd);								// 创建窗口及窗口小部件
static BOOL DestroyAppData(HINSTANCE hInstance);


/*函数实现*/
int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	InitializeCrashRptEx((CRASHRPTPROC)CrashProc);	// 初始异常报告
	if (InitAppData(hInstance, lpCmdLine) == FALSE) return 3;
	if (RegMyWndCls() == FALSE) return 1;
	if (CreateMyWnd(nShowCmd) == FALSE)
	{
		UnregisterClass(g_wsWndClsName, hInstance);
		return 2;
	}
	MainMsgProc();
	DestroyAppData(hInstance);
	return 0;
}

static BOOL InitAppData(HINSTANCE hInstance, LPSTR lpCmdLine)
{
	g_i32MainWndWidth = 450;
	g_i32MainWndHeight = 305;
	wcscpy(g_wsWndClsName, L"AutoBuildWnd");
	for (INT32 _i = 0; _i < MAX_WNDWIDGETS_COUNT; _i++)
		g_struSubWnd[_i].clear();
	g_bIsMainThreadExit = FALSE;

	InitializeCriticalSection(&g_csMainThreadMutex);
	g_hTaskThread = (HANDLE)_beginthreadex(NULL, 0, RunTaskThreadFunc, NULL, 0, NULL);
	g_hControlThread = (HANDLE)_beginthreadex(NULL, 0, RunControlThreadFunc, NULL, 0, NULL);
	if (g_hTaskThread == NULL || g_hTaskThread == INVALID_HANDLE_VALUE)
	{
		WriteRunningLog(TRUE, L"创建任务线程失败，错误码为%u！", GetLastError());
		return FALSE;
	}
	if (g_hControlThread == NULL || g_hControlThread == INVALID_HANDLE_VALUE)
	{
		WriteRunningLog(TRUE, L"创建控制线程失败，错误码为%u！", GetLastError());
		return FALSE;
	}

	wchar_t wsFindApp[128] = L"";
	wchar_t *pFindDot = NULL;
	GetModuleBaseName(GetCurrentProcess(), NULL, wsFindApp, sizeof(wsFindApp)/sizeof(wchar_t));
	pFindDot = wcsstr(wsFindApp, L".exe");
	*pFindDot = 0;
	GetPrivateProfileString(wsFindApp, L"title", L"ERROR", g_wsMainWndTitle, sizeof(g_wsMainWndTitle)/sizeof(wchar_t), L".\\config.ini");
	if (wcscmp(g_wsMainWndTitle, L"ERROR") == 0)
		return FALSE;

	return TRUE;
}

static BOOL RegMyWndCls(void)
{
	WNDCLASSEX  wce;
	memset(&wce, 0, sizeof(WNDCLASSEX));
	wce.cbSize = sizeof(wce);  
	wce.style = CS_VREDRAW | CS_HREDRAW;  
	wce.lpfnWndProc = &MainWndMsgProc;				//指明回调函数  
	wce.hInstance = GetModuleHandle(NULL);  
	wce.hIcon = LoadIcon(0, MAKEINTRESOURCE(IDB_BITMAP1));  
	wce.hCursor = LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));  
	wce.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wce.lpszClassName = g_wsWndClsName;				//独一无二的类名  
	wce.hIconSm = wce.hIcon;  
	return 0 != RegisterClassEx(&wce);  
}

static BOOL CreateMyWnd(int nShowCmd)
{
	RECT rc;
	wchar_t wsTitle[128] = L"";
	INT32 i32SrnWidth = 0;
	INT32 i32SrnHeight = 0;
	DWORD dwWndStyleEx = 0;
	DWORD dwWndStyle = WS_OVERLAPPEDWINDOW &~ (WS_THICKFRAME|WS_MAXIMIZEBOX);
	SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID) &rc, 0);
	i32SrnWidth = rc.right - rc.left;
	i32SrnHeight = rc.bottom - rc.top;
	INT32 i32WndPosX = (i32SrnWidth - g_i32MainWndWidth) / 2 ;
	INT32 i32WndPosY = (i32SrnHeight - g_i32MainWndHeight) / 2;
	wsprintf(wsTitle, L"%s - 未加载执行脚本", g_wsMainWndTitle);

	// 创建主窗口
	g_hThisWndHandle = CreateWindowEx(
		dwWndStyleEx, 
		g_wsWndClsName, 
		wsTitle, 
		dwWndStyle,  
		i32WndPosX, 
		i32WndPosY, 
		g_i32MainWndWidth, 
		g_i32MainWndHeight,  
		NULL, 
		NULL, 
		GetModuleHandle(NULL), 
		NULL);  

	if (NULL == g_hThisWndHandle) 
		return FALSE;
	else
	{
		ShowWindow(g_hThisWndHandle, nShowCmd);
		UpdateWindow(g_hThisWndHandle); 
	}

	// 创建Log窗口
	g_struSubWnd[0].m_hWnd = CreateWindowEx(
		NULL, 
		L"LISTBOX",
		L"Test",
		WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_EXTENDEDSEL,
		27,
		127,
		390,
		94,
		g_hThisWndHandle,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[0].m_hWnd != NULL)
	{
		SetWndFont(g_struSubWnd[0].m_hWnd, L"黑体", FW_NORMAL, 12);
		g_struSubWnd[0].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[0].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[0].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[0].m_hWnd);
		for (char _i = 0; _i < 10; _i++)
			WriteRunningLog(TRUE, L"请点击\"加载脚本\"按钮选择执行的脚本文件!");
	}

	// 创建加载脚本按钮
	g_struSubWnd[1].m_hWnd = CreateWindowEx(
		NULL, 
		L"BUTTON",
		L"加载脚本",
		WS_VISIBLE|WS_CHILD|BS_CENTER|BS_NOTIFY|BS_TOP/*|BS_TEXT*/,
		17,
		224,
		410,
		20,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[1].m_hWnd != NULL)
	{
		g_struSubWnd[1].m_eType = ECONTROLTYPE_BUTTON;
		SetWndFont(g_struSubWnd[1].m_hWnd, L"雅黑", FW_BOLD, 12);
		g_struSubWnd[1].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[0].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[0].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[0].m_hWnd);
	}

	// 创建开始按钮
	g_struSubWnd[2].m_hWnd = CreateWindowEx(
		NULL, 
		L"BUTTON",
		L"开始",
		WS_VISIBLE|WS_CHILD|BS_CENTER|BS_NOTIFY|BS_TOP|BS_TEXT,
		17,
		248 ,
		126,
		20,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[2].m_hWnd != NULL)
	{
		g_struSubWnd[2].m_eType = ECONTROLTYPE_BUTTON;
		SetWndFont(g_struSubWnd[2].m_hWnd, L"雅黑", FW_BOLD, 12);
		g_struSubWnd[2].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[2].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[2].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[2].m_hWnd);
	}

	// 创建暂停按钮
	g_struSubWnd[3].m_hWnd = CreateWindowEx(
		NULL, 
		L"BUTTON",
		L"暂停",
		WS_VISIBLE|WS_CHILD|BS_CENTER|BS_NOTIFY|BS_TOP|BS_TEXT,
		159,
		248,
		126,
		20,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[3].m_hWnd != NULL)
	{
		g_struSubWnd[3].m_eType = ECONTROLTYPE_BUTTON;
		SetWndFont(g_struSubWnd[3].m_hWnd, L"雅黑", FW_BOLD, 12);
		g_struSubWnd[3].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[3].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[3].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[3].m_hWnd);
	}

	// 创建停止按钮
	g_struSubWnd[4].m_hWnd = CreateWindowEx(
		NULL, 
		L"BUTTON",
		L"停止",
		WS_CHILD|WS_VISIBLE|BS_CENTER|BS_NOTIFY|BS_TOP|BS_TEXT,
		301,
		248,
		126,
		20,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[4].m_hWnd != NULL)
	{
		g_struSubWnd[4].m_eType = ECONTROLTYPE_BUTTON;
		SetWndFont(g_struSubWnd[4].m_hWnd, L"雅黑", FW_BOLD, 12);
		g_struSubWnd[4].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[4].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[4].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[4].m_hWnd);
	}

	// 创建总进度条
	g_struSubWnd[5].m_hWnd = CreateWindowEx(
		NULL,
		PROGRESS_CLASS,
		L"总进度",
		WS_CHILD|WS_VISIBLE|PBS_MARQUEE|PBS_SMOOTH,
		27,
		94,
		390,
		15,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[5].m_hWnd != NULL)
	{
		SendMessage(g_struSubWnd[5].m_hWnd, PBM_SETPOS, (WPARAM)0, 0);
		ShowWindow(g_struSubWnd[5].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[5].m_hWnd);
	}

	// 创建当前进度条
	g_struSubWnd[6].m_hWnd = CreateWindowEx(
		NULL,
		PROGRESS_CLASS,
		L"当前进度",
		WS_CHILD|WS_VISIBLE|PBS_MARQUEE|PBS_SMOOTH,
		27,
		37,
		390,
		15,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[6].m_hWnd != NULL)
	{
		SendMessage(g_struSubWnd[6].m_hWnd, PBM_SETPOS, (WPARAM)0, 0);
		ShowWindow(g_struSubWnd[6].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[6].m_hWnd);
	}

	// 创建上小框
	g_struSubWnd[7].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"",
		WS_CHILD|WS_VISIBLE|WS_DISABLED|SS_CENTER|SS_NOTIFY|SS_BLACKFRAME,
		22,
		17,
		400,
		97,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[7].m_hWnd != NULL)
	{
		ShowWindow(g_struSubWnd[7].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[7].m_hWnd);
	}

	// 创建下小框
	g_struSubWnd[8].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"",
		WS_CHILD|WS_VISIBLE|WS_DISABLED|SS_CENTER|SS_NOTIFY|SS_BLACKFRAME,
		22,
		122,
		400,
		82,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[8].m_hWnd != NULL)
	{
		ShowWindow(g_struSubWnd[8].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[8].m_hWnd);
	}

	// 创建大框
	g_struSubWnd[9].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"",
		WS_CHILD|WS_VISIBLE|WS_DISABLED|SS_CENTER|SS_NOTIFY|SS_BLACKFRAME,
		17,
		12,
		410,
		197,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[9].m_hWnd != NULL)
	{
		ShowWindow(g_struSubWnd[9].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[9].m_hWnd);
	}

	// 创建选择脚本窗口
	g_struSubWnd[10].m_hWnd = CreateWindowEx(
		dwWndStyleEx,
		L"LISTBOX",
		L"选择执行脚本", 
		dwWndStyle,
		0,
		0,
		300,
		150, 
		NULL, 
		NULL, 
		GetModuleHandle(NULL), 
		NULL);
	if (g_struSubWnd[10].m_hWnd != NULL)
	{
		SetWndFont(g_struSubWnd[10].m_hWnd, L"雅黑", FW_BOLD, 12);
		g_struSubWnd[10].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[10].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
	}

	// 创建当前任务显示信息标签
	g_struSubWnd[11].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"中华人民共和国",
		WS_CHILD|WS_VISIBLE|SS_LEFT|SS_NOTIFY,
		27,
		22,
		330,
		13,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[11].m_hWnd != NULL)
	{
		g_struSubWnd[11].m_eType = ECONTROLTYPE_LABLE;
		SetWndFont(g_struSubWnd[11].m_hWnd, L"黑体", FW_LIGHT, 12);
		g_struSubWnd[11].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[11].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[11].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[11].m_hWnd);
	}

	// 创建当前任务显示信息标签
	g_struSubWnd[12].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"0%",
		WS_CHILD|WS_VISIBLE|SS_RIGHT|SS_NOTIFY,
		377,
		22,
		40,
		13,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[12].m_hWnd != NULL)
	{
		g_struSubWnd[12].m_eType = ECONTROLTYPE_LABLE;
		SetWndFont(g_struSubWnd[12].m_hWnd, L"黑体", FW_LIGHT, 12);
		g_struSubWnd[12].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[12].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[12].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[12].m_hWnd);
	}

	// 创建"已用时间"标签
	g_struSubWnd[13].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"已用时间",
		WS_CHILD|WS_VISIBLE|SS_LEFT|SS_NOTIFY,
		27,
		64,
		80,
		13,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[13].m_hWnd != NULL)
	{
		g_struSubWnd[13].m_eType = ECONTROLTYPE_LABLE;
		SetWndFont(g_struSubWnd[13].m_hWnd, L"黑体", FW_LIGHT, 12);
		g_struSubWnd[13].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[13].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[13].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[13].m_hWnd);
	}

	// 创建当前任务显示信息标签
	g_struSubWnd[14].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"00:00:00",
		WS_CHILD|WS_VISIBLE|SS_RIGHT|SS_NOTIFY,
		347,
		64,
		70,
		13,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[14].m_hWnd != NULL)
	{
		g_struSubWnd[14].m_eType = ECONTROLTYPE_LABLE;
		SetWndFont(g_struSubWnd[14].m_hWnd, L"黑体", FW_LIGHT, 12);
		g_struSubWnd[14].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[14].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[14].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[14].m_hWnd);
	}

	// 创建"总进度"标签
	g_struSubWnd[15].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"总进度",
		WS_CHILD|WS_VISIBLE|SS_LEFT|SS_NOTIFY,
		27,
		79,
		80,
		13,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[15].m_hWnd != NULL)
	{
		g_struSubWnd[15].m_eType = ECONTROLTYPE_LABLE;
		SetWndFont(g_struSubWnd[15].m_hWnd, L"黑体", FW_LIGHT, 12);
		g_struSubWnd[15].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[15].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[15].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[15].m_hWnd);
	}

	// 创建总进度比例标签
	g_struSubWnd[16].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"0%",
		WS_CHILD|WS_VISIBLE|SS_RIGHT|SS_NOTIFY,
		377,
		79,
		40,
		13,
		g_hThisWndHandle,
		NULL, 
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[16].m_hWnd != NULL)
	{
		g_struSubWnd[16].m_eType = ECONTROLTYPE_LABLE;
		SetWndFont(g_struSubWnd[16].m_hWnd, L"黑体", FW_LIGHT, 12);
		g_struSubWnd[16].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[16].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[16].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[16].m_hWnd);
	}

	// 创建Log窗口
	g_struSubWnd[17].m_hWnd = CreateWindowEx(
		NULL, 
		L"LISTBOX",
		L"Log窗口右键弹出框",
		WS_CHILD|WS_BORDER|LBS_NOTIFY|LBS_EXTENDEDSEL,
		0,
		0,
		100,
		50,
		g_hThisWndHandle,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	if (g_struSubWnd[17].m_hWnd != NULL)
	{
		SetWndFont(g_struSubWnd[17].m_hWnd, L"黑体", FW_NORMAL, 12);
		g_struSubWnd[17].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[17].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		SendMessage(g_struSubWnd[17].m_hWnd, LB_ADDSTRING, NULL, (WPARAM)L"  打开日志文件");
		SendMessage(g_struSubWnd[17].m_hWnd, LB_ADDSTRING, NULL, (WPARAM)L"  清空显示日志");
	}

	return TRUE;
}

static BOOL DestroyAppData(HINSTANCE hInstance)
{
	// 等待线程的退出
	g_bIsMainThreadExit = TRUE;
	if (g_hTaskThread != NULL && g_hTaskThread != INVALID_HANDLE_VALUE)
		WaitForSingleObject(g_hTaskThread, INFINITE);
	if (g_hControlThread != NULL && g_hControlThread != INVALID_HANDLE_VALUE)
		WaitForSingleObject(g_hControlThread, INFINITE);

	// 销毁临界区
	DeleteCriticalSection(&g_csMainThreadMutex);

	// 销毁窗口
	UnregisterClass(g_wsWndClsName, hInstance);

	// 销毁字体资源
	
	return TRUE;
}

BOOL WriteRunningLog(BOOL bIsWriteFile, wchar_t *wsFormat, ...)
{
	va_list valTemp;
	static INT32 i32Index = 0;
	wchar_t wsLog[1024] = L"";
	wchar_t wsLog1[1024] = L"";
	wchar_t wsLog2[1024] = L"";
	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	va_start(valTemp, wsFormat);
	vswprintf(wsLog, wsFormat, valTemp);
	wsprintf(wsLog1, L"%d. %s", ++i32Index, wsLog);
	wsprintf(wsLog2, L"[%02d:%02d:%02d.%03d] %s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, wsLog);

	// 界面输出
	if (g_struSubWnd[0].m_hWnd != NULL)
		SendMessage(g_struSubWnd[0].m_hWnd, LB_ADDSTRING, NULL, (WPARAM)wsLog1);
	else
		return FALSE;

	// 写日志文件
	if (bIsWriteFile == TRUE)
	{
		static BOOL bIsFristOpen = TRUE;
		if (bIsFristOpen == TRUE)
		{
			FILE *pNewFile = fopen(".\\AutoBuild.log", "w+");
			if (pNewFile == NULL)
				return FALSE;
			unsigned char szBuf[4] = "";
			szBuf[0] = 0xff;
			szBuf[1] = 0xfe;
			fwrite(szBuf, 1, 2, pNewFile);
			fclose(pNewFile);
			bIsFristOpen = FALSE;
		}

		FILE *pFile = fopen(".\\AutoBuild.log", "ab+");
		if (pFile == NULL)
			return FALSE;
		fwrite(wsLog2, sizeof(wchar_t), wcslen(wsLog2), pFile);
		fclose(pFile);
	}
	return TRUE;
}

BOOL SetWndFont(HWND hWnd, wchar_t *pName, INT32 i32Weight, INT32 i32Height)
{
//	if (pName == NULL) return FALSE;

	// 设置字体参数
	LOGFONT LogFont;
	::memset(&LogFont, 0, sizeof(LOGFONT));
	wcscpy(LogFont.lfFaceName, pName);
	LogFont.lfWeight = i32Weight;
	LogFont.lfHeight = i32Height;
	LogFont.lfCharSet = GB2312_CHARSET;
	LogFont.lfOutPrecision = OUT_CHARACTER_PRECIS;
	LogFont.lfClipPrecision = CLIP_STROKE_PRECIS;
	LogFont.lfOrientation = 0;
	LogFont.lfQuality = DRAFT_QUALITY;
	LogFont.lfPitchAndFamily = VARIABLE_PITCH;

	// 创建字体
	HFONT hFont = CreateFontIndirect(&LogFont);
	if (hFont == NULL)
		return FALSE;

	// 设置控件字体
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0);

	// 加入销毁链表

	return TRUE;
}