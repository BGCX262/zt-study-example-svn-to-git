/*����ͷ�ļ�*/
#include "ztAutoBuildTool.h"
#include <string>
using namespace std;


/*ȫ�ֱ�������*/
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


/*��������*/
static BOOL InitAppData(HINSTANCE hInstance, LPSTR lpCmdLine);		// ��ʼ����������
static BOOL RegMyWndCls(void);										// ע�ᴰ����
static BOOL CreateMyWnd(int nShowCmd);								// �������ڼ�����С����
static BOOL DestroyAppData(HINSTANCE hInstance);


/*����ʵ��*/
int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	InitializeCrashRptEx((CRASHRPTPROC)CrashProc);	// ��ʼ�쳣����
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
		WriteRunningLog(TRUE, L"���������߳�ʧ�ܣ�������Ϊ%u��", GetLastError());
		return FALSE;
	}
	if (g_hControlThread == NULL || g_hControlThread == INVALID_HANDLE_VALUE)
	{
		WriteRunningLog(TRUE, L"���������߳�ʧ�ܣ�������Ϊ%u��", GetLastError());
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
	wce.lpfnWndProc = &MainWndMsgProc;				//ָ���ص�����  
	wce.hInstance = GetModuleHandle(NULL);  
	wce.hIcon = LoadIcon(0, MAKEINTRESOURCE(IDB_BITMAP1));  
	wce.hCursor = LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));  
	wce.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wce.lpszClassName = g_wsWndClsName;				//��һ�޶�������  
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
	wsprintf(wsTitle, L"%s - δ����ִ�нű�", g_wsMainWndTitle);

	// ����������
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

	// ����Log����
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
		SetWndFont(g_struSubWnd[0].m_hWnd, L"����", FW_NORMAL, 12);
		g_struSubWnd[0].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[0].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[0].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[0].m_hWnd);
		for (char _i = 0; _i < 10; _i++)
			WriteRunningLog(TRUE, L"����\"���ؽű�\"��ťѡ��ִ�еĽű��ļ�!");
	}

	// �������ؽű���ť
	g_struSubWnd[1].m_hWnd = CreateWindowEx(
		NULL, 
		L"BUTTON",
		L"���ؽű�",
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
		SetWndFont(g_struSubWnd[1].m_hWnd, L"�ź�", FW_BOLD, 12);
		g_struSubWnd[1].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[0].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[0].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[0].m_hWnd);
	}

	// ������ʼ��ť
	g_struSubWnd[2].m_hWnd = CreateWindowEx(
		NULL, 
		L"BUTTON",
		L"��ʼ",
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
		SetWndFont(g_struSubWnd[2].m_hWnd, L"�ź�", FW_BOLD, 12);
		g_struSubWnd[2].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[2].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[2].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[2].m_hWnd);
	}

	// ������ͣ��ť
	g_struSubWnd[3].m_hWnd = CreateWindowEx(
		NULL, 
		L"BUTTON",
		L"��ͣ",
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
		SetWndFont(g_struSubWnd[3].m_hWnd, L"�ź�", FW_BOLD, 12);
		g_struSubWnd[3].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[3].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[3].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[3].m_hWnd);
	}

	// ����ֹͣ��ť
	g_struSubWnd[4].m_hWnd = CreateWindowEx(
		NULL, 
		L"BUTTON",
		L"ֹͣ",
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
		SetWndFont(g_struSubWnd[4].m_hWnd, L"�ź�", FW_BOLD, 12);
		g_struSubWnd[4].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[4].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[4].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[4].m_hWnd);
	}

	// �����ܽ�����
	g_struSubWnd[5].m_hWnd = CreateWindowEx(
		NULL,
		PROGRESS_CLASS,
		L"�ܽ���",
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

	// ������ǰ������
	g_struSubWnd[6].m_hWnd = CreateWindowEx(
		NULL,
		PROGRESS_CLASS,
		L"��ǰ����",
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

	// ������С��
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

	// ������С��
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

	// �������
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

	// ����ѡ��ű�����
	g_struSubWnd[10].m_hWnd = CreateWindowEx(
		dwWndStyleEx,
		L"LISTBOX",
		L"ѡ��ִ�нű�", 
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
		SetWndFont(g_struSubWnd[10].m_hWnd, L"�ź�", FW_BOLD, 12);
		g_struSubWnd[10].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[10].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
	}

	// ������ǰ������ʾ��Ϣ��ǩ
	g_struSubWnd[11].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"�л����񹲺͹�",
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
		SetWndFont(g_struSubWnd[11].m_hWnd, L"����", FW_LIGHT, 12);
		g_struSubWnd[11].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[11].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[11].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[11].m_hWnd);
	}

	// ������ǰ������ʾ��Ϣ��ǩ
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
		SetWndFont(g_struSubWnd[12].m_hWnd, L"����", FW_LIGHT, 12);
		g_struSubWnd[12].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[12].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[12].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[12].m_hWnd);
	}

	// ����"����ʱ��"��ǩ
	g_struSubWnd[13].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"����ʱ��",
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
		SetWndFont(g_struSubWnd[13].m_hWnd, L"����", FW_LIGHT, 12);
		g_struSubWnd[13].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[13].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[13].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[13].m_hWnd);
	}

	// ������ǰ������ʾ��Ϣ��ǩ
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
		SetWndFont(g_struSubWnd[14].m_hWnd, L"����", FW_LIGHT, 12);
		g_struSubWnd[14].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[14].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[14].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[14].m_hWnd);
	}

	// ����"�ܽ���"��ǩ
	g_struSubWnd[15].m_hWnd = CreateWindowEx(
		NULL,
		L"STATIC",
		L"�ܽ���",
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
		SetWndFont(g_struSubWnd[15].m_hWnd, L"����", FW_LIGHT, 12);
		g_struSubWnd[15].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[15].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[15].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[15].m_hWnd);
	}

	// �����ܽ��ȱ�����ǩ
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
		SetWndFont(g_struSubWnd[16].m_hWnd, L"����", FW_LIGHT, 12);
		g_struSubWnd[16].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[16].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		ShowWindow(g_struSubWnd[16].m_hWnd, nShowCmd);
		UpdateWindow(g_struSubWnd[16].m_hWnd);
	}

	// ����Log����
	g_struSubWnd[17].m_hWnd = CreateWindowEx(
		NULL, 
		L"LISTBOX",
		L"Log�����Ҽ�������",
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
		SetWndFont(g_struSubWnd[17].m_hWnd, L"����", FW_NORMAL, 12);
		g_struSubWnd[17].m_lpfnWnd = (WNDMSGCALLBACKPROC)SetWindowLong(g_struSubWnd[17].m_hWnd, GWL_WNDPROC, (LONG)SubWndMsgProc);
		SendMessage(g_struSubWnd[17].m_hWnd, LB_ADDSTRING, NULL, (WPARAM)L"  ����־�ļ�");
		SendMessage(g_struSubWnd[17].m_hWnd, LB_ADDSTRING, NULL, (WPARAM)L"  �����ʾ��־");
	}

	return TRUE;
}

static BOOL DestroyAppData(HINSTANCE hInstance)
{
	// �ȴ��̵߳��˳�
	g_bIsMainThreadExit = TRUE;
	if (g_hTaskThread != NULL && g_hTaskThread != INVALID_HANDLE_VALUE)
		WaitForSingleObject(g_hTaskThread, INFINITE);
	if (g_hControlThread != NULL && g_hControlThread != INVALID_HANDLE_VALUE)
		WaitForSingleObject(g_hControlThread, INFINITE);

	// �����ٽ���
	DeleteCriticalSection(&g_csMainThreadMutex);

	// ���ٴ���
	UnregisterClass(g_wsWndClsName, hInstance);

	// ����������Դ
	
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

	// �������
	if (g_struSubWnd[0].m_hWnd != NULL)
		SendMessage(g_struSubWnd[0].m_hWnd, LB_ADDSTRING, NULL, (WPARAM)wsLog1);
	else
		return FALSE;

	// д��־�ļ�
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

	// �����������
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

	// ��������
	HFONT hFont = CreateFontIndirect(&LogFont);
	if (hFont == NULL)
		return FALSE;

	// ���ÿؼ�����
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0);

	// ������������

	return TRUE;
}