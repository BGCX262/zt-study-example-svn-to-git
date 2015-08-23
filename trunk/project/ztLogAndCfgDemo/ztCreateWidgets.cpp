/*包含头文件及依赖库*/
#include <vector>
using namespace std;
#include "common.h"
#include "ztLogAndCfgDemo.h"


/*全局变量*/
WNDPROC g_lpfnLogWndProc = NULL;
//CModifyConfig cConfigWnd;
//CModifyConfig g_cLineLossTableWnd;
//CModifyConfig g_cLimitsWnd;
vector<CModifyConfig> g_cModifyCfgWnd;


/*内部函数申明*/
static BOOL __CreateConfigWnd(void);
static BOOL __CreateLineLossTableWnd(void);
static BOOL __CreateLimitsWnd(void);


/*函数实现*/
BOOL RegisterDemoClass(void)
{
	DWORD dwLastError = 0;
	WNDCLASSEXW wc;
	memset(&wc, 0, sizeof(WNDCLASSEXW));
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = MainMsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hThisInstance;
	wc.hIcon = LoadIcon(g_hThisInstance, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));/*(HBRUSH)GetStockObject(LTGRAY_BRUSH);*/
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_wsWndClsName;
	wc.hIconSm = NULL;
	if (RegisterClassEx(&wc) == 0)
	{
		dwLastError = GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL UnRegisterDemoClass()
{
	if (g_hThisInstance == NULL)
		return FALSE;
	return UnregisterClass(g_wsWndClsName, g_hThisInstance);
}

BOOL CreateDemoWindow(void)
{
	// 创建窗口
	if (g_hFatherWnd == NULL && CreateMainWidgets() == FALSE)
		return FALSE;
	if (CreateLogWidgets() == FALSE)
		return FALSE;
	if (CreateCfgWidgets() == FALSE)
		return FALSE;

	// 创建logic线程
	unsigned ui32ThreadId = 0;
	g_hLogicThread = (HANDLE)_beginthreadex(NULL, 0, ThreadLogic, NULL, 0, &ui32ThreadId);
	if (g_hLogicThread == NULL)
		return FALSE;

	return TRUE;
}

BOOL CreateMainWidgets(void)
{
	DWORD dwLastError = 0;
	INT32 i32InitWndWidth = 600;	// 初始宽
	INT32 i32InitWndHeight = 480;	// 初始高
	INT32 i32InitWndPosX = (GetSystemMetrics(SM_CXSCREEN) - i32InitWndWidth) / 2;
	INT32 i32InitWndPosY = (GetSystemMetrics(SM_CYSCREEN) - i32InitWndHeight) / 2;

	g_hMainWnd = CreateWindow(g_wsWndClsName, 
		L"Log&Cfg",
		WS_OVERLAPPEDWINDOW,
		i32InitWndPosX,
		i32InitWndPosY,
		i32InitWndWidth,
		i32InitWndHeight,
		NULL,
		NULL,
		g_hThisInstance,
		NULL);

	if (g_hMainWnd == NULL)
	{
		dwLastError = GetLastError();
		return FALSE;
	}

	if (g_hFatherWnd == NULL)
	{
		ShowWindow(g_hMainWnd, g_iStatusInitShow);  
		UpdateWindow(g_hMainWnd);
	}
	else
	{
		ShowWindow(g_hMainWnd, SW_HIDE);  
	}

	return TRUE;
}

BOOL CreateLogWidgets(void)
{
	if (g_hMainWnd == NULL && g_hFatherWnd == NULL)
		return FALSE;

	DWORD dwLastError = 0;
	g_hLogWnd = CreateWindow(L"LISTBOX",  
		NULL,  
		WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY,  
		20, 225, 518, 157,   
		(g_hFatherWnd == NULL ? g_hMainWnd : g_hFatherWnd), 
		NULL,   
		g_hThisInstance,   
		NULL);
	if (g_hLogWnd == NULL)
	{
		dwLastError = GetLastError();
		return FALSE;
	}

	SetWndFont(g_hLogWnd, L"Calibri", FW_MEDIUM, 12, 6);
	ShowWindow(g_hLogWnd, SW_SHOW);
	g_lpfnLogWndProc = (WNDPROC)SetWindowLong(g_hLogWnd, GWL_WNDPROC, ((LONG)WidgetsMsgProc));
	return TRUE;
}

static BOOL __CreateConfigWnd(void)
{
	wchar_t wsCfgFullName[256] = L"";
	wchar_t* wsSectionName[7] = {L"GeneralConfig", 
		L"DeviceConfig", 
		L"UEComConfig",
		L"LogConfig",
		L"BandChannelConfig",
		L"TestConfig",
		L"TestProduce"};
	wsprintf(wsCfgFullName, L"%s%s", g_wsThisInstancePath, L"QualcommLTENSTestConfig.ini");
	CModifyConfig cConfigWnd;
	if (cConfigWnd.InitSection(g_hCfgWnd, wsCfgFullName, wsSectionName, 7) == TRUE)
	{
		cConfigWnd.AddItem(L"DeviceConfig", L"GPIBID_PHONE1");
		cConfigWnd.AddItem(L"DeviceConfig", L"GPIBPriAddr_PHONE1");
		cConfigWnd.AddItem(L"DeviceConfig", L"GPIBID_PHONE2");
		cConfigWnd.AddItem(L"DeviceConfig", L"GPIBPriAddr_PHONE2");
		cConfigWnd.AddItem(L"DeviceConfig", L"TCPIPAddr_PHONE1");
		cConfigWnd.AddItem(L"DeviceConfig", L"TCPIPAddr_PHONE2");

		cConfigWnd.AddItem(L"GeneralConfig", L"UseParallelPhones");
		cConfigWnd.AddItem(L"GeneralConfig", L"AutoDetectDeviceAddr");
		cConfigWnd.AddItem(L"GeneralConfig", L"DeviceControlMode");
		cConfigWnd.AddItem(L"GeneralConfig", L"AutoDetectUECom");
		cConfigWnd.AddItem(L"GeneralConfig", L"LogPathConfig");
		cConfigWnd.AddItem(L"GeneralConfig", L"SchemeMode");
		cConfigWnd.AddItem(L"GeneralConfig", L"ReSync");
		cConfigWnd.AddItem(L"GeneralConfig", L"RetestCount");
		cConfigWnd.AddItem(L"GeneralConfig", L"FailToContinue");

		cConfigWnd.AddItem(L"UEComConfig", L"Com_PHONE1");
		cConfigWnd.AddItem(L"UEComConfig", L"Com_PHONE2");

		cConfigWnd.AddItem(L"LogConfig", L"SaveFilePath");
		cConfigWnd.AddItem(L"LogConfig", L"ProcessLogName");
		cConfigWnd.AddItem(L"LogConfig", L"ResultLogName");
		cConfigWnd.AddItem(L"LogConfig", L"LogType");

		cConfigWnd.AddItem(L"BandChannelConfig", L"TestBandCount");
		cConfigWnd.AddItem(L"BandChannelConfig", L"TestBand1_ChannelCount");
		cConfigWnd.AddItem(L"BandChannelConfig", L"TestBand1_Channel");
		cConfigWnd.AddItem(L"BandChannelConfig", L"TestBand1_RxPort");

		cConfigWnd.AddItem(L"TestConfig", L"ACLRPower");
		cConfigWnd.AddItem(L"TestConfig", L"ACLRatio");
		cConfigWnd.AddItem(L"TestConfig", L"ACLRPassFail");
		cConfigWnd.AddItem(L"TestConfig", L"ACLRTargetPower");
		cConfigWnd.AddItem(L"TestConfig", L"OBWValue");
		cConfigWnd.AddItem(L"TestConfig", L"OBWPassFail");
		cConfigWnd.AddItem(L"TestConfig", L"OBWTargetPower");
		cConfigWnd.AddItem(L"TestConfig", L"SEMPassFail");
		cConfigWnd.AddItem(L"TestConfig", L"SEMWorstY");
		cConfigWnd.AddItem(L"TestConfig", L"SEMTargetPower");
		cConfigWnd.AddItem(L"TestConfig", L"FerrPassFail");
		cConfigWnd.AddItem(L"TestConfig", L"FerrFer");
		cConfigWnd.AddItem(L"TestConfig", L"FerrPPM");
		cConfigWnd.AddItem(L"TestConfig", L"FerrTargetPower");
		cConfigWnd.AddItem(L"TestConfig", L"TXPPower");
		cConfigWnd.AddItem(L"TestConfig", L"TXPPassFail");
		cConfigWnd.AddItem(L"TestConfig", L"TXPTargetPower");
		cConfigWnd.AddItem(L"TestConfig", L"RPTTrace");
		cConfigWnd.AddItem(L"TestConfig", L"RPTPassFail");
		cConfigWnd.AddItem(L"TestConfig", L"TXPTargetPower");
		cConfigWnd.AddItem(L"TestConfig", L"TSQPuschEVM");
		cConfigWnd.AddItem(L"TestConfig", L"TSQEVMDMRS");
		cConfigWnd.AddItem(L"TestConfig", L"TSQCarrierLeakage");
		cConfigWnd.AddItem(L"TestConfig", L"TSQRipple");
		cConfigWnd.AddItem(L"TestConfig", L"TSQFerr");
		cConfigWnd.AddItem(L"TestConfig", L"TSQTargetPower");
		cConfigWnd.AddItem(L"TestConfig", L"TSQ2PuschEVM");
		cConfigWnd.AddItem(L"TestConfig", L"TSQ2EVMDMRS");
		cConfigWnd.AddItem(L"TestConfig", L"TSQ2CarrierLeakage");
		cConfigWnd.AddItem(L"TestConfig", L"TSQ2Ferr");
		cConfigWnd.AddItem(L"TestConfig", L"TSQ2Ripple");
		cConfigWnd.AddItem(L"TestConfig", L"TSQ2TargetPower");
		cConfigWnd.AddItem(L"TestConfig", L"MIOPPower");
		cConfigWnd.AddItem(L"TestConfig", L"MIOPTestMode");
		cConfigWnd.AddItem(L"TestConfig", L"MIOPPassFail");
		cConfigWnd.AddItem(L"TestConfig", L"MIOPTargetPower");
		cConfigWnd.AddItem(L"TestConfig", L"RX_Mode");
		cConfigWnd.AddItem(L"TestConfig", L"RX_RSRP");

		cConfigWnd.AddItem(L"TestProduce", L"TestBandwidth");
		cConfigWnd.AddItem(L"TestProduce", L"TestCheckConnect");
		cConfigWnd.AddItem(L"TestProduce", L"TestLogState");
		INT32 i32TestCount = GetPrivateProfileInt(L"TestProduce", L"TestCount", 0, wsCfgFullName);
		for (INT32 _i = 0; _i < i32TestCount; _i++)
		{
			wchar_t wsKeyName[32] = L"";
			wchar_t wsKeyValue[256] = L"";
			wsprintf(wsKeyName, L"Test%d", _i+1);
			cConfigWnd.AddItem(L"TestProduce", wsKeyName);
		}
		g_cModifyCfgWnd.push_back(cConfigWnd);
	}

	return GetLastError() == 0;
}

static BOOL __CreateLineLossTableWnd(void)
{
	wchar_t wsCfgFullName[256] = L"";
	wchar_t* wsSectionName[4] = {L"LinelossConfig", 
		L"LineLossTable", 
		L"LineLossBand",
		L"LineLossFreq"};
	wsprintf(wsCfgFullName, L"%s%s", g_wsThisInstancePath, L"Lineloss.ini");
	CModifyConfig cConfigWnd;
	if (cConfigWnd.InitSection(g_hLineLossWnd, wsCfgFullName, wsSectionName, 4) == TRUE)
	{
		cConfigWnd.AddItem(L"LinelossConfig", L"Mode");

		INT32 i32Count = GetPrivateProfileInt(L"LineLossTable", L"LineCount", 0, wsCfgFullName);
		for (INT32 _i = 0; _i < i32Count; _i++)
		{
			wchar_t wsKeyName[32] = L"";
			wchar_t wsKeyValue[256] = L"";
			wsprintf(wsKeyName, L"Line%d", _i+1);
			cConfigWnd.AddItem(L"LineLossTable", wsKeyName);
		}

		i32Count = GetPrivateProfileInt(L"LineLossBand", L"BandCount", 0, wsCfgFullName);
		for (INT32 _i = 0; _i < i32Count; _i++)
		{
			wchar_t wsKeyName[32] = L"";
			wchar_t wsKeyValue[256] = L"";
			wsprintf(wsKeyName, L"Band%d", _i+1);
			cConfigWnd.AddItem(L"LineLossBand", wsKeyName);
		}

		i32Count = GetPrivateProfileInt(L"LineLossFreq", L"FreqCount", 0, wsCfgFullName);
		for (INT32 _i = 0; _i < i32Count; _i++)
		{
			wchar_t wsKeyName[32] = L"";
			wchar_t wsKeyValue[256] = L"";
			wsprintf(wsKeyName, L"Freq%d", _i+1);
			cConfigWnd.AddItem(L"LineLossFreq", wsKeyName);
		}

		g_cModifyCfgWnd.push_back(cConfigWnd);
	}

	return GetLastError() == 0;
}

static BOOL __CreateLimitsWnd(void)
{
	wchar_t wsCfgFullName[256] = L"";
	wchar_t* wsSectionName[1] = {L"Limits"};
	wsprintf(wsCfgFullName, L"%s%s", g_wsThisInstancePath, L"Limits.ini");
	CModifyConfig cConfigWnd;
	if (cConfigWnd.InitSection(g_hLimitsWnd, wsCfgFullName, wsSectionName, 1) == TRUE)
	{
		SetLastError(0);
		INT32 i32Count = GetPrivateProfileInt(L"Content", L"count", 0, wsCfgFullName);
		DWORD dwLastError = GetLastError();
		for (INT32 _i = 0; _i < i32Count; _i++)
		{
			wchar_t wsKeyName[32] = L"";
			wchar_t wsKeyValue[256] = L"";
			wchar_t wsKeyNameTemp[32] = L"";
			wsprintf(wsKeyNameTemp, L"%d", _i);
			if (GetPrivateProfileString(L"Content", wsKeyNameTemp, L"ERROR", wsKeyName, sizeof(wsKeyName), wsCfgFullName) != 0)
				cConfigWnd.AddItem(L"Limits", wsKeyName);
		}

		g_cModifyCfgWnd.push_back(cConfigWnd);
	}

	return GetLastError() == 0;
}

BOOL CreateCfgWidgets(void)
{
	if (g_hFatherWnd == NULL)
		return FALSE;
	if (__CreateConfigWnd() == FALSE)
		return FALSE;
	if (__CreateLineLossTableWnd() == FALSE)
		return FALSE;
	if (__CreateLimitsWnd() == FALSE)
		return FALSE;
	
	return TRUE;
}

BOOL DestroyDemoWindow(void)
{
	// 等待Logic线程退出
	if (g_hLogicThread != INVALID_HANDLE_VALUE && g_hLogicThread != NULL)
	{
		DWORD dwExitCode = 0;
		WaitForSingleObject(g_hLogicThread, INFINITE);
		GetExitCodeThread(g_hLogicThread, &dwExitCode);
		CloseHandle(g_hLogicThread);
		g_hLogicThread = INVALID_HANDLE_VALUE;
	}

	// 销毁字体资源
	size_t stCount = g_vFontRes.size();
	for (size_t _i = 0; _i < stCount; _i++)
	{
		DeleteObject(g_vFontRes[_i]);
	}
	g_vFontRes.clear();

	// 销毁窗口
	size_t stCfgCount = g_cModifyCfgWnd.size();
	for (size_t _i = 0; _i < stCfgCount; _i++)
	{
		g_cModifyCfgWnd[_i].DelItem();
		g_cModifyCfgWnd[_i].DestroySection();
	}
	g_cModifyCfgWnd.clear();

	if (g_hLogWnd != NULL && DestroyWindow(g_hLogWnd) == FALSE)
		return FALSE;
	if (g_hMainWnd != NULL && DestroyWindow(g_hMainWnd) == FALSE)
		return FALSE;
	return TRUE;
}

void MainMsgPeek(void)
{
	// 消息拾取
	MSG msg;  
	ZeroMemory(&msg, sizeof(msg));  
	do
	{  
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))  
		{  
			TranslateMessage(&msg);  
			DispatchMessage(&msg);  
		}
		else
			Sleep(1);
	}while(msg.message != WM_QUIT);
}

BOOL InitAppData(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 获取当前进程所在路径
	wchar_t *pwsFind = NULL;
	wchar_t wsThisAppFullName[256] = L"";
	wchar_t wsThisAppFullRevName[256] = L"";
	GetModuleFileName(g_hThisInstance, wsThisAppFullName, sizeof(wsThisAppFullName));
	wcscpy(wsThisAppFullRevName, wcsrev(wsThisAppFullName));
	pwsFind = wcsstr(wsThisAppFullRevName, L"\\");
	if (pwsFind == NULL)
	{
		pwsFind = wcsstr(wsThisAppFullRevName, L"/");
		if (pwsFind == NULL)
		{
			return FALSE;
		}
	}
	wcscpy(g_wsThisInstancePath, wcsrev(pwsFind));

	// 接收进程相关参数
	g_bIsMainThreadExit = FALSE;
	g_bIsSaveLogToFile = FALSE;
	g_hThisInstance = hInstance;
	g_iStatusInitShow = nCmdShow;
	wcscpy(g_wsWndClsName, L"QualcommLTENSTest");
	wcscpy(g_wsThisCfgName, L"QualcommLTENSTestConfig.ini");
	wsprintf(g_wsThisCfgFullPath, L"%s%s", g_wsThisInstancePath, g_wsThisCfgName);

	// 清空全局变量
	g_cModifyCfgWnd.clear();
	return TRUE;
}

BOOL SetWndFont(HWND hWnd, wchar_t *pName, INT32 i32Weight, INT32 i32Height, INT32 i32Width)
{
	if (pName == NULL) return FALSE;

	// 设置字体参数
	LOGFONT LogFont;
	::memset(&LogFont, 0, sizeof(LOGFONT));
	wcscpy(LogFont.lfFaceName, pName);
	LogFont.lfWeight = i32Weight;
	LogFont.lfHeight = i32Height;
	LogFont.lfWidth = i32Width;
	LogFont.lfCharSet = GB2312_CHARSET;
	LogFont.lfOutPrecision = OUT_TT_PRECIS;
	LogFont.lfOrientation = 0;
	LogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	LogFont.lfQuality = CLEARTYPE_QUALITY;
	LogFont.lfPitchAndFamily = FIXED_PITCH|FF_MODERN;

	// 创建字体
	HFONT hFont = CreateFontIndirect(&LogFont);
	if (hFont == NULL)
		return FALSE;

	// 设置控件字体
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0);

	// 加入销毁链表
	g_vFontRes.push_back(hFont);
	return TRUE;
}


CModifyConfig::CModifyConfig()
{
	m_vSection.clear();
	m_hBaseboard = NULL;
	m_hShowBaseboard = NULL;
	m_i32CurWndWidth = 0;
	m_i32CurWndHeight = 0;
	memset(m_wsCfgFileName, 0, sizeof(m_wsCfgFileName));
}

CModifyConfig::~CModifyConfig()
{

}

BOOL CModifyConfig::AddItem(wchar_t* wsSectionName, wchar_t* wsKeyName)
{
	//  查找Section所在位置
	size_t stIndex = -1;
	size_t stCount = m_vSection.size();
	for (size_t _i = 0; _i < stCount; _i++)
	{
		if (wcsicmp(wsSectionName, m_vSection[_i].m_wsSectionName) == 0)
		{
			stIndex = _i;
			break;
		}
	}

	// 若不存在则直接返回
	if (stIndex == -1)
		return FALSE;

	// 增加一个Key，创建外框
	STRUKEYINFO struKey;
	INT32 i32FramePosX = 10 + (m_vSection[stIndex].m_vKey.size()%2)*((m_vSection[stIndex].m_i32SectionWidth-30 + 20)/2 + 5) + 1;
	INT32 i32FramePosY = (m_vSection[stIndex].m_vKey.size()/2) * 16 + 10;
	INT32 i32FrameWidth = (m_vSection[stIndex].m_i32SectionWidth-30)/2 - 5;
	struKey.m_hFrame = CreateWindow(L"STATIC", 
		L"", 
		WS_CHILD|WS_VISIBLE/*|SS_ETCHEDFRAME*/,
		i32FramePosX,
		i32FramePosY,
		i32FrameWidth,
		16,
		m_vSection[stIndex].m_hSectionFrame,
		NULL,
		g_hThisInstance,
		NULL);
	if (struKey.m_hFrame == NULL)
		return FALSE;
	ShowWindow(struKey.m_hFrame, SW_SHOW);

	// 创建key中等号前的控件
	wcscpy(struKey.m_wsKey, wsKeyName);
	struKey.m_hKey = CreateWindow(L"STATIC", 
		struKey.m_wsKey, 
		WS_CHILD|WS_VISIBLE|WS_DISABLED,
		1,
		2,
		(i32FrameWidth - 2) * 5 / 11,
		12,
		struKey.m_hFrame,
		NULL,
		g_hThisInstance,
		NULL);
	if (struKey.m_hKey == NULL)
		return FALSE;
	ShowWindow(struKey.m_hKey, SW_SHOW);
	SetWndFont(struKey.m_hKey, L"Calibri", FW_MEDIUM, 12, 6);

	// 创建Key中等号后可编辑的控件
	GetPrivateProfileString(wsSectionName, wsKeyName, L"N/A", struKey.m_wsKeyValue, sizeof(struKey.m_wsKeyValue), m_wsCfgFileName);
	struKey.m_hKeyValue = CreateWindow(L"EDIT", 
		struKey.m_wsKeyValue, 
		WS_CHILD|WS_VISIBLE|ES_LEFT|ES_AUTOHSCROLL,
		1 + (i32FrameWidth - 2) * 5 / 11,
		2,
		(i32FrameWidth - 2) * 6 / 11,
		12,
		struKey.m_hFrame,
		NULL,
		g_hThisInstance,
		NULL);
	if (struKey.m_hKeyValue == NULL)
		return FALSE;
	ShowWindow(struKey.m_hKeyValue, SW_SHOW);
	SetWndFont(struKey.m_hKeyValue, L"Calibri", FW_MEDIUM, 12, 6);
	struKey.m_pfnCallback = (WNDPROC)SetWindowLong(struKey.m_hKeyValue, GWL_WNDPROC, (LONG)WidgetsMsgProc);

	// 将创建的key压入容器
	m_vSection[stIndex].m_vKey.push_back(struKey);

	if (m_vSection[stIndex].m_vKey.size() % 2 != 0)
	{
		m_i32CurContentHeight += 16;

		// 修改此Section窗口大小
		m_vSection[stIndex].m_i32SectionHeight += 16;
		SetWindowPos(m_vSection[stIndex].m_hSectionFrame, 
			NULL, 
			0, 
			0,
			m_vSection[stIndex].m_i32SectionWidth, 
			m_vSection[stIndex].m_i32SectionHeight, 
			SWP_NOMOVE|SWP_NOZORDER);

		// 移动此Section下方控件
		for (size_t _i = stIndex+1; _i <stCount; _i++)
		{
			m_vSection[_i].m_i32SectionPosY += 16;
			SetWindowPos(m_vSection[_i].m_hSectionFrame,
				NULL,
				m_vSection[_i].m_i32SectionPosX, 
				m_vSection[_i].m_i32SectionPosY, 
				0, 
				0, 
				SWP_NOSIZE|SWP_NOZORDER);
			SetWindowPos(m_vSection[_i].m_hSectionCaption, 
				NULL, 
				m_vSection[_i].m_i32SectionPosX+20, 
				m_vSection[_i].m_i32SectionPosY-6, 
				0, 
				0, 
				SWP_NOSIZE|SWP_NOZORDER);
		}
	}
	
	return TRUE;
}

BOOL CModifyConfig::DelItem(void)
{
	size_t stSectionCount = m_vSection.size();
	for (size_t _i = 0; _i < stSectionCount; _i++)
	{
		size_t stKeyCount = m_vSection[_i].m_vKey.size();
		for (size_t _j = 0; _j < stKeyCount; _j++)
		{
			DestroyWindow(m_vSection[_i].m_vKey[_j].m_hKeyValue);
			DestroyWindow(m_vSection[_i].m_vKey[_j].m_hKey);
			DestroyWindow(m_vSection[_i].m_vKey[_j].m_hFrame);
		}
		m_vSection[_i].m_vKey.clear();
	}
	m_vSection.clear();

	return TRUE;
}

BOOL CModifyConfig::InitSection(HWND &hWnd, wchar_t *wsCfgFileName, wchar_t* wsSectionName[], INT32 i32SectionCount)
{
	// 创建Config主界面
	wchar_t wsWndName[128] = L"";
	m_i32CurWndWidth = GetPrivateProfileInt(L"Wnd", L"WndWidth", 480, wsCfgFileName);
	m_i32CurWndHeight = GetPrivateProfileInt(L"Wnd", L"WndHeight", 580, wsCfgFileName);
	GetPrivateProfileString(L"Wnd", L"WndName", L"!!!NoCfg!!!", wsWndName, sizeof(wsWndName), wsCfgFileName);
	hWnd = CreateWindow(g_wsWndClsName, 
		wsWndName,
		WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VSCROLL/*|WS_CHILD*/,
		0,
		0,
		m_i32CurWndWidth,
		m_i32CurWndHeight,
		g_hFatherWnd,
		NULL,
		g_hThisInstance,
		NULL);
	if (hWnd == NULL)
		return FALSE;
	ShowWindow(hWnd, SW_HIDE);
	m_hBaseboard = hWnd;
	SetScrollRange(hWnd, SB_VERT, 0, 1000, TRUE);
	m_lpfnDefaultMsgProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WidgetsMsgProc);

	// 创建显示底板
	m_hShowBaseboard = CreateWindow(g_wsWndClsName, 
		L"",
		WS_CHILD,
		-2,
		-0,
		m_i32CurWndWidth,
		m_i32CurWndHeight*10,
		m_hBaseboard,
		NULL,
		g_hThisInstance,
		NULL);
	if (hWnd == NULL)
		return FALSE;
	ShowWindow(m_hShowBaseboard, SW_SHOW);
	SetWindowLong(m_hShowBaseboard, GWL_WNDPROC, (LONG)WidgetsMsgProc);

	// 创建主界面上各Section，作为主界面的子窗口
	STRUSECTIONINFO struSection;
	struSection.m_vKey.clear();
	wcscpy(m_wsCfgFileName, wsCfgFileName);
	for (INT32 _i = 0; _i < i32SectionCount; _i++)
	{
		struSection.m_i32SectionWidth = m_i32CurWndWidth - 40;
		struSection.m_i32SectionHeight = 12;
		struSection.m_i32SectionPosX = 10;
		struSection.m_i32SectionPosY = 15 + _i * 25;
		wcscpy(struSection.m_wsSectionName, wsSectionName[_i]);
		struSection.m_hSectionFrame = CreateWindow(L"STATIC",
			struSection.m_wsSectionName,
			WS_CHILD|WS_VISIBLE|SS_BLACKFRAME,
			struSection.m_i32SectionPosX,
			struSection.m_i32SectionPosY,
			struSection.m_i32SectionWidth,
			struSection.m_i32SectionHeight,
			m_hShowBaseboard,
			NULL,
			g_hThisInstance,
			NULL);
		if (struSection.m_hSectionFrame == NULL)
			continue;
		ShowWindow(struSection.m_hSectionFrame, SW_SHOW);

		struSection.m_hSectionCaption = CreateWindow(L"STATIC",
			struSection.m_wsSectionName,
			WS_CHILD|WS_VISIBLE|SS_CENTER,
			struSection.m_i32SectionPosX + 20,
			struSection.m_i32SectionPosY - 6,
			wcslen(struSection.m_wsSectionName) * 7 + 2,
			14,
			m_hShowBaseboard,
			NULL,
			g_hThisInstance,
			NULL);
		if (struSection.m_hSectionCaption == NULL)
			continue;
		ShowWindow(struSection.m_hSectionCaption, SW_SHOW);
		SetWndFont(struSection.m_hSectionCaption, L"Calibri", FW_MEDIUM, 12, 6);

		m_vSection.push_back(struSection);
	}

	m_i32CurShowPosY = 0;
	if (i32SectionCount > 0)
		m_i32CurContentHeight = m_vSection[i32SectionCount-1].m_i32SectionPosY + m_vSection[i32SectionCount-1].m_i32SectionHeight + 40;

	return TRUE;
}

BOOL CModifyConfig::DestroySection(void)
{
	DWORD dwLastError = 0;
	UINT32 ui32FailedCount = 0;
	size_t stCount = m_vSection.size();
	for (size_t _i = 0; _i < stCount; _i++)
	{
		if (DestroyWindow(m_vSection[_i].m_hSectionCaption) == FALSE)
		{
			ui32FailedCount++;
			dwLastError = GetLastError();
		}

		if (DestroyWindow(m_vSection[_i].m_hSectionFrame) == FALSE)
		{
			ui32FailedCount++;
			dwLastError = GetLastError();
		}
	}
	m_vSection.clear();

	if (DestroyWindow(m_hShowBaseboard) == FALSE)
	{
		ui32FailedCount++;
		dwLastError = GetLastError();
	}
	m_hShowBaseboard = NULL;

	if (DestroyWindow(m_hBaseboard) == FALSE)
	{
		ui32FailedCount++;
		dwLastError = GetLastError();
	}
	m_hBaseboard = NULL;

	return (ui32FailedCount == 0);
}

BOOL CModifyConfig::ShowConfigWnd(void)
{
	if (m_hBaseboard == NULL)
		return FALSE;

	ShowWindow(m_hBaseboard, SW_SHOW);
	return TRUE;
}

BOOL CModifyConfig::HideConfigWnd(void)
{
	if (m_hBaseboard == NULL)
		return FALSE;

	ShowWindow(m_hBaseboard, SW_HIDE);
	return TRUE;
}

HWND CModifyConfig::GetBaseboardWnd(void) 
{
	return m_hBaseboard;
}

HWND CModifyConfig::GetShowBaseboardWnd(void)
{
	return m_hShowBaseboard;
}

void CModifyConfig::UpdateClientShow(INT32 i32ShowPos)
{
	INT32 i32ShowMinPos = 0;
	INT32 i32ShowMaxPos = 0;
	GetScrollRange(m_hBaseboard, SB_VERT, &i32ShowMinPos, &i32ShowMaxPos);
	if (i32ShowMinPos > i32ShowPos || i32ShowMaxPos < i32ShowPos)
		return;

	if (m_i32CurWndHeight >= m_i32CurContentHeight)
		return;

	INT32 i32ShowPosY = 0;
	i32ShowPosY = - (i32ShowPos-i32ShowMinPos) * (m_i32CurContentHeight - m_i32CurWndHeight) / (i32ShowMaxPos - i32ShowMinPos);
	SetWindowPos(m_hShowBaseboard, NULL, -2, i32ShowPosY, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
}

BOOL CModifyConfig::SaveCfg(void)
{
	LARGE_INTEGER liFreq;
	LARGE_INTEGER liBegin;
	LARGE_INTEGER liEnd;
	QueryPerformanceFrequency(&liFreq);
	QueryPerformanceCounter(&liBegin);
	INT32 i32WriteFailedCount = 0;
	INT32 i32WriteSucceedCount = 0;
	size_t stSectionCount = m_vSection.size();
	for (size_t _i = 0; _i < stSectionCount; _i++)
	{
		size_t stKeyCount = m_vSection[_i].m_vKey.size();
		for (size_t _j = 0; _j < stKeyCount; _j++)
		{
			// 获取编辑框当前的内容，并判断是否为空
			wchar_t wsBuf[256] = L"";
			if (GetWindowText(m_vSection[_i].m_vKey[_j].m_hKeyValue, wsBuf, sizeof(wsBuf)) == 0)
			{
				DWORD dwLastError = GetLastError();
				if (dwLastError == 0)
				{
					i32WriteFailedCount++;
					wchar_t wsLog[128] = L"";
					wsprintf(wsLog, L"Please check \"%s\\%s\"field!!!",
						m_vSection[_i].m_wsSectionName, m_vSection[_i].m_vKey[_j].m_wsKey);
					AddLogToWnd(wsLog, 0.0);
				}
				continue;
			}

			// 对比编辑框内容和内存中保存的内容，若不同，则保存，用编辑框的内容覆盖内存及ini文件中的内容
			if (wcscmp(wsBuf, m_vSection[_i].m_vKey[_j].m_wsKeyValue) == 0)
			{
				// 清空编辑框的修改状态
				SendMessage(m_vSection[_i].m_vKey[_j].m_hKeyValue, EM_SETMODIFY, (WPARAM)FALSE, NULL);
				continue;
			}

			// 写ini文件
			if (WritePrivateProfileString(m_vSection[_i].m_wsSectionName, 
				m_vSection[_i].m_vKey[_j].m_wsKey, 
				wsBuf,
				m_wsCfgFileName) == FALSE)
			{
				i32WriteFailedCount++;
				continue;
			}

			// 修改内存中保存的内容
			wcscpy(m_vSection[_i].m_vKey[_j].m_wsKeyValue, wsBuf);

			// 若成功则计数+1
			i32WriteSucceedCount++;

			// 清空编辑框的修改状态
			SendMessage(m_vSection[_i].m_vKey[_j].m_hKeyValue, EM_SETMODIFY, (WPARAM)FALSE, NULL);
		}
	}

	wchar_t wsLog[128] = L"";
	if (i32WriteFailedCount != 0)
		wsprintf(wsLog, L"ERROR:Write config file failed! %d changes of failure.", i32WriteFailedCount, i32WriteSucceedCount);
	else if (i32WriteSucceedCount != 0)
		wsprintf(wsLog, L"Successfully written to the config file! %d changes of success.", i32WriteSucceedCount);
	else
		wsprintf(wsLog, L"This config file without modification.");
	QueryPerformanceCounter(&liEnd);
	double dbCostTime = (double)(liEnd.QuadPart-liBegin.QuadPart)*1000.0/(double)liFreq.QuadPart;
	AddLogToWnd(wsLog, dbCostTime);
	return i32WriteFailedCount == 0;
}

INT32 CModifyConfig::GetUnsaveCount(void)
{
	INT32 i32ModifiedCount = 0;
	size_t stSectionCount = m_vSection.size();
	for (size_t _m = 0; _m < stSectionCount; _m++)
	{
		size_t stKeyCount = m_vSection[_m].m_vKey.size();
		for (size_t _n = 0; _n < stKeyCount; _n++)
		{
			BOOL bIsModified = SendMessage(m_vSection[_m].m_vKey[_n].m_hKeyValue, EM_GETMODIFY, NULL, NULL);
			if (bIsModified != 0)
				i32ModifiedCount++;
		}
	}

	return i32ModifiedCount;
}

LRESULT CALLBACK CModifyConfig::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL sbIsCtrl = FALSE;
	switch (msg)
	{
	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case 'S':
				if (sbIsCtrl == TRUE)
				{
					PostMessage(m_hBaseboard, msg, wParam, lParam);
					return 0;
				}
				break;

			case VK_CONTROL:
				sbIsCtrl = TRUE;
				PostMessage(m_hBaseboard, msg, wParam, lParam);
				break;
			}
		}
		break;

	case WM_KEYUP:
		{
			switch (wParam)
			{
			case VK_CONTROL:
				sbIsCtrl = FALSE;
				PostMessage(m_hBaseboard, msg, wParam, lParam);
				return 0;
			}
		}
		break;
	}

	BOOL bIsBreak = FALSE;
	size_t stSectionCount = m_vSection.size();
	for (size_t _i = 0; _i < stSectionCount; _i++)
	{
		size_t stKeyCount = m_vSection[_i].m_vKey.size();
		for (size_t _j = 0; _j < stKeyCount; _j++)
		{
			if (m_vSection[_i].m_vKey[_j].m_hKeyValue == hWnd)
			{
				return m_vSection[_i].m_vKey[_j].m_pfnCallback(hWnd, msg, wParam, lParam);
			}
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}