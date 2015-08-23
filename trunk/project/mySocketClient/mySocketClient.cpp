#include "mySocketClient.h"


/*全局变量定义*/
wchar_t g_wsClassName[128] = L"";								// 注册窗口类名
HWND g_hThisWndHandle= NULL;									// 主窗口句柄
HINSTANCE g_hThisAppInstance = NULL;							// 实例句柄
INT32 g_i32MainWndWidth = 0;									// 主窗口宽
INT32 g_i32MainWndHeight = 0;									// 主窗口高
UINT32 g_ui32ServerPort = -1;									// 服务器端口号
wchar_t g_wsServerIpAddr[128] = L"";							// 服务器Ip地址
BOOL g_bIsMainThreadExit = FALSE;								// 主线程退出标志位,此标志位位TRUE时其他线程必须退出
HANDLE g_hLogicThread = INVALID_HANDLE_VALUE;					// 逻辑线程句柄
HANDLE g_hRenderThread = INVALID_HANDLE_VALUE;					// 渲染线程句柄


/*函数实现*/
int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	InitializeCrashRptEx((CRASHRPTPROC)CrashProc);				// 初始异常报告
	ProcCmdLine(lpCmdLine);										// 处理命令行
	InitAppData(hInstance);										// 初始化应用程序数据
	if (RegisterMyClass() == FALSE) 
		return 1;
	if (CreateMyWindow(nShowCmd) == FALSE)
	{
		UnregisterClass(g_wsClassName, hInstance);
		return 2;
	}

	MainMsgProc();
	DestroyAppData();
	return 0;
}

INT32 ProcCmdLine(LPSTR lpCmdLine)
{
	return 0;
}

BOOL InitAppData(HINSTANCE hInstance)
{
	// 变量初始化
	g_hThisAppInstance = hInstance;
	wcscpy(g_wsClassName, L"SocketDemoClientWnd");
	g_i32MainWndWidth = GetPrivateProfileInt(L"mySocketClient", L"width", DEFAULTWNDWEITH, L".\\config.ini");
	g_i32MainWndHeight = GetPrivateProfileInt(L"mySocketClient", L"height", DEFAULTWNDHEIGHT, L".\\config.ini");
	g_ui32ServerPort = GetPrivateProfileInt(L"mySocketClient", L"port", 0, L".\\config.ini");
	GetPrivateProfileString(L"mySocketClient", L"ip", L"", g_wsServerIpAddr, sizeof(g_wsServerIpAddr), L".\\config.ini");

	// 初始化网络资源
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		wchar_t wsError[128] = L"";
		wsprintf(wsError, L"初始化网络资源失败!\n错误码为%u!", GetLastError());
		ShowErrorMessage(wsError);
		return FALSE;
	}
	if (LOBYTE(wsa.wVersion) != 2 || HIBYTE(wsa.wVersion) != 2)
	{
		wchar_t wsError[128] = L"";
		wsprintf(wsError, L"网络资源版本不匹配!\n%d,%d",LOBYTE(wsa.wVersion), HIBYTE(wsa.wVersion));
		ShowErrorMessage(wsError);
		WSACleanup();
		return FALSE;
	}

	// 线程初始化
	unsigned int ui32LogicThreadID = 0;
	unsigned int ui32RenderThreadID = 0;
	g_hLogicThread = (HANDLE)_beginthreadex(NULL, 0, LogicThread, NULL, 0, &ui32LogicThreadID);
	g_hRenderThread = (HANDLE)_beginthreadex(NULL, 0, RenderThread, NULL, 0, &ui32RenderThreadID);
	return TRUE;
}

void DestroyAppData(void)
{
	// 等待子线程退出
	g_bIsMainThreadExit = TRUE;
	if (g_hLogicThread != INVALID_HANDLE_VALUE && g_hLogicThread != NULL)
	{
		WaitForSingleObject(g_hLogicThread, INFINITE); 
		CloseHandle(g_hLogicThread);
	}

	if (g_hRenderThread != INVALID_HANDLE_VALUE && g_hRenderThread != NULL)
	{
		WaitForSingleObject(g_hRenderThread, INFINITE);
		CloseHandle(g_hRenderThread);
	}

	// 销毁网络资源
	WSACleanup();

	// 销毁窗口类
	UnregisterClass(g_wsClassName, g_hThisAppInstance);
}

BOOL RegisterMyClass(void) 
{  
	WNDCLASSEX  wce;
	memset(&wce, 0, sizeof(WNDCLASSEX));
	wce.cbSize = sizeof(wce);  
	wce.style = CS_VREDRAW | CS_HREDRAW;  
	wce.lpfnWndProc = &MainWndMsgProc;				//指明回调函数  
	wce.hInstance = GetModuleHandle(NULL);  
	wce.hIcon = LoadIcon(0, MAKEINTRESOURCE(IDI_WINLOGO));  
	wce.hCursor = LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));  
	wce.hbrBackground = (HBRUSH)GetStockObject(BLACK_PEN);
	wce.lpszClassName = g_wsClassName;				//独一无二的类名  
	wce.hIconSm = wce.hIcon;  
	return 0 != RegisterClassEx(&wce);  
}

BOOL CreateMyWindow(INT32 cmdShow) 
{  
	DWORD dwWndStyle = 0;
	DWORD dwWndStyleEx = 0;
	INT32 i32SrnWidth = GetSystemMetrics(SM_CXSCREEN);
	INT32 i32SrnHeight = GetSystemMetrics(SM_CYSCREEN);
	INT32 i32WndPosX = (i32SrnWidth - g_i32MainWndWidth) / 2 > 0 ? (i32SrnWidth - g_i32MainWndWidth) / 2 : 0;
	INT32 i32WndPosY = (i32SrnHeight - g_i32MainWndHeight) / 2 > 0 ? (i32SrnHeight - g_i32MainWndHeight) / 2 : 0;
	if (GetPrivateProfileInt(L"wnd", L"border", DEFAULTWNDWEITH, L".\\config.ini"))
		dwWndStyle = WS_OVERLAPPEDWINDOW;
	else
		dwWndStyle = WS_POPUP;
	if (GetPrivateProfileInt(L"wnd", L"istop", DEFAULTWNDWEITH, L".\\config.ini"))
		dwWndStyleEx = WS_EX_TOPMOST;

	// 窗口长或宽任意为0则全屏显示
	if (g_i32MainWndWidth == 0  || g_i32MainWndHeight == 0)
	{
		g_i32MainWndWidth = i32SrnWidth;
		g_i32MainWndHeight = i32SrnHeight;
		i32WndPosX = 0;
		i32WndPosY = 0;
		dwWndStyle = WS_POPUP;
	}

	g_hThisWndHandle = CreateWindowEx(
		dwWndStyleEx, 
		g_wsClassName, 
		L"ClientTest", 
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
		ShowWindow(g_hThisWndHandle, cmdShow);
		UpdateWindow(g_hThisWndHandle); 
	}

	return TRUE;
}

void MainMsgProc(void)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	
	do 
	{
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}while(msg.message != WM_QUIT);
}

void ConnectServer(SOCKET &skt)
{
	char szServerIpAddr[128] = "";
	wcstombs(szServerIpAddr, g_wsServerIpAddr, sizeof(szServerIpAddr));
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=inet_addr(szServerIpAddr);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(g_ui32ServerPort);

	skt = socket(AF_INET, SOCK_STREAM, 0);
	connect(skt, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
}