#include "ztAutoBuildTool.h"


/*函数声明*/


/*函数实现*/
BOOL WINAPI CrashProc(PEXCEPTION_POINTERS Exception)
{   
	// 获取当前进程所在路径
	wchar_t *pFindPath = NULL;
	wchar_t wsModeleBaseName[256] = L"";
	wchar_t wsCrashXmlFileName[256] = L"";
	wchar_t wsCrashDumpFileName[256] = L"";
	wchar_t wsModeleReverseName[256] = L"";
	GetModuleFileNameEx(GetCurrentProcess(), NULL, wsModeleBaseName, sizeof(wsModeleBaseName)/sizeof(wchar_t));
	wcscpy(wsModeleReverseName, wcsrev(wsModeleBaseName));
	for (INT16 _i = 0; _i < 256; _i++)
	{
		if (wsModeleReverseName[_i] == 0)
			break;
		if (wsModeleReverseName[_i] == L'/')
			wsModeleReverseName[_i] = L'\\';
	}
	pFindPath = wcsstr(wsModeleReverseName, L"\\");
	wcscpy(wsModeleBaseName, wcsrev(pFindPath));
	wcscat(wsModeleBaseName, L"CrashRpt\\");

	// 创建对应文件目录
	SYSTEMTIME st;
	wchar_t wsDatePath[256];
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	wsprintf(wsDatePath, L"%s%04d-%02d-%02d\\", wsModeleBaseName, st.wYear, st.wMonth, st.wDay);
	CreateDirectory(wsModeleBaseName, NULL);
	CreateDirectory(wsDatePath, NULL);

	// 生成文件路径
	wsprintf(wsCrashXmlFileName, L"%sCrashRpt_%02d%02d%02d.xml", wsDatePath, st.wHour, st.wMinute, st.wSecond);
	wsprintf(wsCrashDumpFileName, L"%sCrashRpt_%02d%02d%02d.dmp", wsDatePath, st.wHour, st.wMinute, st.wSecond);

	//生成错误时系统快照
	GenerateCrashRpt(Exception, wsCrashXmlFileName, CRASHRPT_ERROR|CRASHRPT_SYSTEM|CRASHRPT_PROCESS|CRASHRPT_SOFTWARE|CRASHRPT_HARDWARE);

	//生成minidump.dmp，这个可以用vs2008或者windbg打开分析
	GenerateMiniDump(Exception, wsCrashDumpFileName);

	// 作为发布给其他用户使用的版本，此处还应有Dump发送工具，可通过邮件发送。

	// 弹框提示
	wchar_t wsOutput[512] = L"";
	wchar_t wsThisInsName[256] = L"";
	GetModuleBaseName(GetCurrentProcess(), NULL, wsThisInsName, sizeof(wsThisInsName)/sizeof(wchar_t));
	wsprintf(wsOutput, L"哦哦！应用程序%s已崩溃！\n请保存：\n%s\n%s", wsThisInsName, wsCrashXmlFileName, wsCrashDumpFileName);
	MessageBox(NULL, wsOutput, L"ERROR", MB_OK);

	return EXCEPTION_EXECUTE_HANDLER;
}

LRESULT CALLBACK MainWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	static HBRUSH hStaticBrush;
	switch (msg) 
	{
	case WM_CREATE:
		hStaticBrush = CreateSolidBrush(RGB(192, 192, 192));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			break;

		case VK_DOWN:
			break;

		case VK_LEFT:
			break;

		case VK_RIGHT:
			break;
		}
		break;

	case WM_SETFOCUS:
		if (IsWindowVisible(g_struSubWnd[10].m_hWnd) == TRUE)
			SetFocus(g_struSubWnd[10].m_hWnd);
		break;

	case WM_KILLFOCUS:
		break;

	case WM_KEYUP:
		if(wParam == VK_ESCAPE) 
		{
			PostQuitMessage(0);
		}
		break;

	case WM_MOVE:
		break;

	case WM_SIZE:
		break;

	case WM_LBUTTONDOWN:
		if (g_struSubWnd[17].m_hWnd != hWnd)
		{
			HideLogControlWnd();
		}
		break;
 
	case WM_COMMAND:
		{
			if (BN_CLICKED == (int)LOWORD(wParam))
			{
				if ((HANDLE)lParam == g_struSubWnd[1].m_hWnd)		// 点击“加载脚本”按钮
				{
					ShowLoadScriptWnd(TRUE);		
				}
				else if ((HANDLE)lParam == g_struSubWnd[2].m_hWnd)	// 点击“开始”按钮
				{
					EnterCriticalSection(&g_csMainThreadMutex);
					if (g_eCurrentTaskStatus == ETASKSTATUS_RUN) 
					{
						LeaveCriticalSection(&g_csMainThreadMutex);
						return 1;
					}
					g_eCurrentTaskStatus = ETASKSTATUS_RUN;
					g_ui32TaskBeginTime = GetTickCount();
					g_ui32TaskPauseTime = 0;
					LeaveCriticalSection(&g_csMainThreadMutex);
				}
				else if ((HANDLE)lParam == g_struSubWnd[3].m_hWnd)	// 点击“暂停”按钮
				{
					EnterCriticalSection(&g_csMainThreadMutex);
					if (g_eCurrentTaskStatus == ETASKSTATUS_RUN)
					{
						SetWindowText(g_struSubWnd[3].m_hWnd, L"暂停");
						g_eCurrentTaskStatus = ETASKSTATUS_PAUSE;
						g_ui32TaskBeginTime = 0;
						g_ui32TaskPauseTime = GetTickCount();
					}
					else if (g_eCurrentTaskStatus == ETASKSTATUS_PAUSE)
					{
						SetWindowText(g_struSubWnd[3].m_hWnd, L"继续");
						g_eCurrentTaskStatus = ETASKSTATUS_RUN;
						g_ui32TaskPauseTime = 0;
					}
					else
					{
						LeaveCriticalSection(&g_csMainThreadMutex);
						return 1;
					}
					LeaveCriticalSection(&g_csMainThreadMutex);
				}
				else if ((HANDLE)lParam == g_struSubWnd[4].m_hWnd)	// 点击“停止”按钮
				{
					EnterCriticalSection(&g_csMainThreadMutex);
					if (g_eCurrentTaskStatus == ETASKSTATUS_STOP) 
					{
						LeaveCriticalSection(&g_csMainThreadMutex);
						return 1;
					}
					g_eCurrentTaskStatus = ETASKSTATUS_STOP;
					g_ui32TaskBeginTime = 0;
					g_ui32TaskPauseTime = 0;
					LeaveCriticalSection(&g_csMainThreadMutex);
				}
			}
		}
		break;

	case WM_SYSCOMMAND:
		break;

	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORSTATIC:
		{
			SetBkColor((HDC)wParam, RGB(192, 192, 192)); 
			return (LRESULT)hStaticBrush;
		}
		break;

	case WM_SYSCOLORCHANGE :
		DeleteObject(hStaticBrush);
		hStaticBrush = CreateSolidBrush(RGB(192, 192, 192)) ;
		return 0 ;

	case WM_DESTROY:
		DeleteObject(hStaticBrush);
		PostQuitMessage(0);
		break;
	} 

	return DefWindowProc(hWnd, msg, wParam, lParam); 
}

LRESULT CALLBACK SubWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SETFOCUS:
		break;

	case WM_KILLFOCUS:
		break;

	case WM_PAINT:
		break;

	case WM_MOVE:
		break;

	case WM_DESTROY:
		break;

	case WM_SYSCOMMAND:
		{
			if (hWnd == g_struSubWnd[10].m_hWnd && (wParam == SC_CLOSE || wParam == SC_MINIMIZE))
			{
				return ShowLoadScriptWnd(FALSE);
			}
		}
		break;

	case WM_LBUTTONDOWN:
		if (g_struSubWnd[17].m_hWnd != hWnd)
		{
			HideLogControlWnd();
		}
		break;

	case WM_RBUTTONUP:
		if (g_struSubWnd[0].m_hWnd == hWnd)		// log窗口上右键
		{
			ShowLogControlWnd();
		}
		break;
	}

	for (INT32 _i = 0; _i < MAX_WNDWIDGETS_COUNT; _i++)
	{
		if (g_struSubWnd[_i].m_hWnd == NULL)
			continue;

		if (hWnd == g_struSubWnd[_i].m_hWnd)
		{
			return g_struSubWnd[_i].m_lpfnWnd(hWnd, msg, wParam, lParam);
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
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
		Sleep(1);
	}while(msg.message != WM_QUIT);
}