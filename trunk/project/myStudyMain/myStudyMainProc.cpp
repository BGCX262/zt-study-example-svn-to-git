#include <iostream>
#include "myStudyMain.h"
using namespace std;


LRESULT CALLBACK MainWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{  
	switch (msg) 
	{ 
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			OnPressUpKey(hWnd, lParam);
			break;

		case VK_DOWN:
			OnPressDownKey(hWnd, lParam);
			break;

		case VK_LEFT:
			OnPressLeftKey(hWnd, lParam);
			break;

		case VK_RIGHT:
			OnPressRightKey(hWnd, lParam);
			break;
		}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT struPS;
			memset(&struPS, 0, sizeof(PAINTSTRUCT));
			HDC hDC = BeginPaint(hWnd, &struPS);
			static UINT64 ui64Output= 0;
			wchar_t wsOutput[128] = L"";
			wsprintf(wsOutput, L"%I64u", ui64Output);
			TextOut(hDC, 20, 80, wsOutput, wcslen(wsOutput));
			EndPaint(hWnd, &struPS);
		}
		break;

	case WM_LBUTTONDOWN:
		{
			int a = 0;
		}
		break;

	case WM_DESTROY:  
		PostQuitMessage(0);							//如果是“窗口销毁”事件，则应该在消息队列中投递  
		break;
	} 

	return DefWindowProc(hWnd, msg, wParam, lParam); 
}

LRESULT CALLBACK InputCtrlMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{ 
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			{
				char szBuffer[512] = "";
				GetWindowTextA(hWnd, szBuffer, sizeof(szBuffer));
				if (stricmp(szBuffer, "exit") == 0)
				{
					PostQuitMessage(0);
				}
			}
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		{
			SetWindowText(hWnd, L"");
		}
		break;

	case WM_DESTROY:  
		PostQuitMessage(0);
		break;
	}

	if (g_lpfnInputWndBackup == NULL)
		return DefWindowProc(hWnd, msg, wParam, lParam); 
	else
		return g_lpfnInputWndBackup(hWnd, msg, wParam, lParam);
}

BOOL WINAPI CrashProc(PEXCEPTION_POINTERS Exception)
{   
	MessageBox(NULL, L"Debug", L"Debug", MB_OK);

	//生成错误时系统快照
	GenerateCrashRpt(Exception, L".\\miniError.xml", CRASHRPT_ERROR|CRASHRPT_SYSTEM|CRASHRPT_PROCESS);

	////生成minidump.dmp，这个可以用vc或者windbg打开分析
	//GenerateMiniDump( Exception, L".\\PSH2dump.dmp" );

	return EXCEPTION_EXECUTE_HANDLER;
}