#include "ztAutoBuildTool.h"


/*��������*/


/*����ʵ��*/
BOOL WINAPI CrashProc(PEXCEPTION_POINTERS Exception)
{   
	// ��ȡ��ǰ��������·��
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

	// ������Ӧ�ļ�Ŀ¼
	SYSTEMTIME st;
	wchar_t wsDatePath[256];
	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	wsprintf(wsDatePath, L"%s%04d-%02d-%02d\\", wsModeleBaseName, st.wYear, st.wMonth, st.wDay);
	CreateDirectory(wsModeleBaseName, NULL);
	CreateDirectory(wsDatePath, NULL);

	// �����ļ�·��
	wsprintf(wsCrashXmlFileName, L"%sCrashRpt_%02d%02d%02d.xml", wsDatePath, st.wHour, st.wMinute, st.wSecond);
	wsprintf(wsCrashDumpFileName, L"%sCrashRpt_%02d%02d%02d.dmp", wsDatePath, st.wHour, st.wMinute, st.wSecond);

	//���ɴ���ʱϵͳ����
	GenerateCrashRpt(Exception, wsCrashXmlFileName, CRASHRPT_ERROR|CRASHRPT_SYSTEM|CRASHRPT_PROCESS|CRASHRPT_SOFTWARE|CRASHRPT_HARDWARE);

	//����minidump.dmp�����������vs2008����windbg�򿪷���
	GenerateMiniDump(Exception, wsCrashDumpFileName);

	// ��Ϊ�����������û�ʹ�õİ汾���˴���Ӧ��Dump���͹��ߣ���ͨ���ʼ����͡�

	// ������ʾ
	wchar_t wsOutput[512] = L"";
	wchar_t wsThisInsName[256] = L"";
	GetModuleBaseName(GetCurrentProcess(), NULL, wsThisInsName, sizeof(wsThisInsName)/sizeof(wchar_t));
	wsprintf(wsOutput, L"ŶŶ��Ӧ�ó���%s�ѱ�����\n�뱣�棺\n%s\n%s", wsThisInsName, wsCrashXmlFileName, wsCrashDumpFileName);
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
				if ((HANDLE)lParam == g_struSubWnd[1].m_hWnd)		// ��������ؽű�����ť
				{
					ShowLoadScriptWnd(TRUE);		
				}
				else if ((HANDLE)lParam == g_struSubWnd[2].m_hWnd)	// �������ʼ����ť
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
				else if ((HANDLE)lParam == g_struSubWnd[3].m_hWnd)	// �������ͣ����ť
				{
					EnterCriticalSection(&g_csMainThreadMutex);
					if (g_eCurrentTaskStatus == ETASKSTATUS_RUN)
					{
						SetWindowText(g_struSubWnd[3].m_hWnd, L"��ͣ");
						g_eCurrentTaskStatus = ETASKSTATUS_PAUSE;
						g_ui32TaskBeginTime = 0;
						g_ui32TaskPauseTime = GetTickCount();
					}
					else if (g_eCurrentTaskStatus == ETASKSTATUS_PAUSE)
					{
						SetWindowText(g_struSubWnd[3].m_hWnd, L"����");
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
				else if ((HANDLE)lParam == g_struSubWnd[4].m_hWnd)	// �����ֹͣ����ť
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
		if (g_struSubWnd[0].m_hWnd == hWnd)		// log�������Ҽ�
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