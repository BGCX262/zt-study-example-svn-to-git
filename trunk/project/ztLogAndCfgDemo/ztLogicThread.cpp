/*����ͷ�ļ���������*/
#include "common.h"
#include "ztLogAndCfgDemo.h"


/*�ڲ���������*/
static BOOL __SetCfgStatus(void);


/*����ʵ��*/
unsigned __stdcall ThreadLogic(void *pPara)
{
	while (TRUE)
	{
		if (g_bIsMainThreadExit == TRUE)									// �ж����߳��Ƿ��˳�
			break;

		__SetCfgStatus();
		Sleep(10);
	}

	return 0;
}

static BOOL __SetCfgStatus(void)
{
	size_t stCount = g_cModifyCfgWnd.size();
	for (size_t _i = 0; _i < stCount; _i++)
	{
		if (g_cModifyCfgWnd[_i].GetUnsaveCount() != 0)
		{
			wchar_t wsCaption[128] = L"";
			wchar_t wsCaptionModify[128] = L"";
			GetWindowText(g_cModifyCfgWnd[_i].GetBaseboardWnd(), wsCaption, sizeof(wsCaption));
			DWORD dwLastError = GetLastError();
			if (wcsstr(wsCaption, L"*") == NULL)
			{
				wsprintf(wsCaptionModify, L"%s*", wsCaption);
				SetWindowText(g_cModifyCfgWnd[_i].GetBaseboardWnd(), wsCaptionModify);
			}
		}
		else
		{
			wchar_t wsCaption[128] = L"";
			wchar_t wsCaptionModify[128] = L"";
			GetWindowText(g_cModifyCfgWnd[_i].GetBaseboardWnd(), wsCaption, sizeof(wsCaption));
			if (wcsstr(wsCaption, L"*") != NULL)
			{
				size_t stCopyLen = (wcsstr(wsCaption, L"*") - wsCaption)*sizeof(wchar_t);
				memcpy(wsCaptionModify, wsCaption, stCopyLen);
				wsCaptionModify[stCopyLen] = 0;
				SetWindowText(g_cModifyCfgWnd[_i].GetBaseboardWnd(), wsCaptionModify);
			}
		}
	}

	return TRUE;
}