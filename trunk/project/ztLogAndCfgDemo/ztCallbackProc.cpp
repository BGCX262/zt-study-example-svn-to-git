/*包含头文件及依赖库*/
#include "common.h"
#include "ztLogAndCfgDemo.h"


/*函数申明*/
static BOOL __UpdateWndByScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);
static BOOL __SaveCfgData(HWND hWnd);
static BOOL __ProcMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam);
static BOOL __HideCurCfgWnd(HWND hWnd, WPARAM wParam, LPARAM lParam);
static BOOL __ProcTableKey(HWND hWnd, WPARAM wParam, LPARAM lParam);


/*函数实现*/
LRESULT CALLBACK MainMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)  
	{
	case WM_DESTROY:
		PostQuitMessage(0);  
		return 0;

	case WM_KEYUP:  
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0); 
		break;
	}  

	return DefWindowProc(hWnd, msg, wParam, lParam);  
}

LRESULT CALLBACK WidgetsMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	static BOOL sbIsCtrlPressping = FALSE;
	switch (msg) 
	{
	case WM_SETFOCUS:
		break;

	case WM_KILLFOCUS:
		break;

	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case 'S':
				if (sbIsCtrlPressping == TRUE)
					__SaveCfgData(hWnd);
				break;

			case VK_CONTROL:
				sbIsCtrlPressping = TRUE;
				break;

			case VK_TAB:
				__ProcTableKey(hWnd, wParam, lParam);
				break;
			}
		}
		break;

	case WM_KEYUP:
		{
			switch (wParam)
			{
			case VK_CONTROL:
				sbIsCtrlPressping = FALSE;
				break;
			}
		}
		break;

	case WM_MOUSEWHEEL:
		__ProcMouseWheel(hWnd, wParam, lParam);
		break;

	case WM_DESTROY:
		{
			if (g_bIsMainThreadExit == FALSE)
			{
				if (__HideCurCfgWnd(hWnd, wParam, lParam) == TRUE)
					return 0;
			}
		}
		break; 

	case WM_VSCROLL:
		if (__UpdateWndByScroll(hWnd, wParam, lParam) == TRUE)
			return 0;
		break;
	}

	// 调用原窗口过程函数或默认窗口过程函数
	if (hWnd == g_hLogWnd)
		return g_lpfnLogWndProc(hWnd, msg, wParam, lParam);
	else if (hWnd == g_hCfgWnd || hWnd == g_hLineLossWnd || hWnd == g_hLimitsWnd)
	{
		for (size_t _i = 0; _i < g_cModifyCfgWnd.size(); _i++)
		{
			if (hWnd == g_cModifyCfgWnd[_i].GetBaseboardWnd())
				return g_cModifyCfgWnd[_i].m_lpfnDefaultMsgProc(hWnd, msg, wParam, lParam);
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	else
	{
		for (size_t _i = 0; _i < g_cModifyCfgWnd.size(); _i++)
		{
			size_t stSectionCount = g_cModifyCfgWnd[_i].m_vSection.size();
			for (size_t _m = 0; _m < stSectionCount; _m++)
			{
				size_t stKeyCount = g_cModifyCfgWnd[_i].m_vSection[_m].m_vKey.size();
				for (size_t _n = 0; _n < stKeyCount; _n++)
				{
					if (hWnd == g_cModifyCfgWnd[_i].m_vSection[_m].m_vKey[_n].m_hKeyValue)
						return g_cModifyCfgWnd[_i].m_vSection[_m].m_vKey[_n].m_pfnCallback(hWnd, msg, wParam, lParam);
				}
			}
		}

		return DefWindowProc(hWnd, msg, wParam, lParam); 
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

static BOOL __UpdateWndByScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	BOOL bIsNeedRet = TRUE;
	size_t stCount = g_cModifyCfgWnd.size();
	for (size_t _i = 0; _i < stCount; _i++)
	{
		if (g_cModifyCfgWnd[_i].GetBaseboardWnd() == hWnd)
		{
			bIsNeedRet = FALSE;
			break;
		}
	}
	if (bIsNeedRet == TRUE)
		return FALSE;

	INT32 i32ShowPos = 0;
	INT32 i32PrevShowPos = 0;
	WORD wHi = HIWORD((DWORD)wParam);
	WORD wLo = LOWORD((DWORD)wParam);
	switch (wLo)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i32ShowPos = wHi;
		i32PrevShowPos = GetScrollPos(hWnd, SB_VERT);
		SetScrollPos(hWnd, SB_VERT, wHi, TRUE);
		break;

	case SB_LINEUP:
		{
			INT32 i32Pos = GetScrollPos(hWnd, SB_VERT);
			i32PrevShowPos = i32Pos;
			INT32 i32MinPos = 0;
			INT32 i32MaxPox = 0;
			GetScrollRange(hWnd, SB_VERT, &i32MinPos, &i32MaxPox);
			INT32 i32Step = (i32MaxPox - i32MinPos) / 10;
			i32Pos = (i32Pos - i32Step) < i32MinPos ? i32MinPos : (i32Pos - i32Step);
			i32ShowPos = i32Pos;
			SetScrollPos(hWnd, SB_VERT, i32Pos, TRUE);
		}
		break;

	case SB_LINEDOWN:
		{
			INT32 i32Pos = GetScrollPos(hWnd, SB_VERT);
			i32PrevShowPos = i32Pos;
			INT32 i32MinPos = 0;
			INT32 i32MaxPox = 0;
			GetScrollRange(hWnd, SB_VERT, &i32MinPos, &i32MaxPox);
			INT32 i32Step = (i32MaxPox - i32MinPos) / 10;
			i32Pos = (i32Pos + i32Step) > i32MaxPox ? i32MaxPox : (i32Pos + i32Step);
			i32ShowPos = i32Pos;
			SetScrollPos(hWnd, SB_VERT, i32Pos, TRUE);
		}
		break;

	case SB_TOP:
		{
			INT32 i32MinPos = 0;
			INT32 i32MaxPox = 0;
			GetScrollRange(hWnd, SB_VERT, &i32MinPos, &i32MaxPox);
			i32PrevShowPos = GetScrollPos(hWnd, SB_VERT);
			i32ShowPos = i32MinPos;
			SetScrollPos(hWnd, SB_VERT, i32MinPos, TRUE);
		}
		break;

	case SB_BOTTOM:
		{
			INT32 i32MinPos = 0;
			INT32 i32MaxPox = 0;
			GetScrollRange(hWnd, SB_VERT, &i32MinPos, &i32MaxPox);
			i32PrevShowPos = GetScrollPos(hWnd, SB_VERT);
			i32ShowPos = i32MaxPox;
			SetScrollPos(hWnd, SB_VERT, i32MaxPox, TRUE);
		}
		break;
	}

	// 如果滚动条移动过则更新显示区域
	if (i32PrevShowPos != i32ShowPos)
	{
		size_t stCount = g_cModifyCfgWnd.size();
		for (size_t _i = 0; _i < stCount; _i++)
		{
			if (g_cModifyCfgWnd[_i].GetBaseboardWnd() == hWnd)
			{
				g_cModifyCfgWnd[_i].UpdateClientShow(i32ShowPos);
				break;
			}
		}
	}

	return TRUE;
}

static BOOL __SaveCfgData(HWND hWnd)
{
	size_t stCount = g_cModifyCfgWnd.size();
	for (size_t _i = 0; _i < stCount; _i++)
	{
		if (hWnd == g_cModifyCfgWnd[_i].GetBaseboardWnd())
		{
			return g_cModifyCfgWnd[_i].SaveCfg();
		}
		else
		{
			HWND hGreatGrandfatherWnd = GetParent(GetParent(GetParent(GetParent(hWnd))));
			if (hGreatGrandfatherWnd == g_cModifyCfgWnd[_i].GetBaseboardWnd())
			{
				return g_cModifyCfgWnd[_i].SaveCfg();
			}
		}
	}
	return FALSE;
}

static BOOL __ProcMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HWND hRatateWnd = NULL;
	char szOutput[128] = "";
	INT16 i16Rotate = (INT16)HIWORD(wParam);
	size_t stCount = g_cModifyCfgWnd.size();
	for (size_t _i = 0; _i < stCount; _i++)
	{
		if (hWnd == g_cModifyCfgWnd[_i].GetBaseboardWnd())
		{
			hRatateWnd = hWnd;
			break;
		}
	}
	if (hRatateWnd != NULL)
	{
		WPARAM wParamSend = 0;
		if (i16Rotate > 0)
		{
			wParamSend = MAKELONG(SB_LINEUP, 0);
		}
		else if (i16Rotate < 0)
		{
			wParamSend = MAKELONG(SB_LINEDOWN, 0);
		}
		PostMessage(hRatateWnd, WM_VSCROLL, wParamSend, NULL);
		return TRUE;
	}
	else
		return FALSE;
}

static BOOL __HideCurCfgWnd(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	size_t stCount = g_cModifyCfgWnd.size();
	for (size_t _i = 0; _i < stCount; _i++)
	{
		if (hWnd == g_cModifyCfgWnd[_i].GetShowBaseboardWnd())
			return TRUE;

		if (hWnd == g_cModifyCfgWnd[_i].GetBaseboardWnd())
		{
			// 检测窗口中所有编辑框是否有内容修改过
			INT32 i32ModifiedCount = g_cModifyCfgWnd[_i].GetUnsaveCount();
			if (i32ModifiedCount != 0)
			{
				wchar_t wsOutput[128] = L"";
				wsprintf(wsOutput, 
					L"There are %d changes have not been saved!\nDo you want to save the changes?\nClick OK save, to give up is Cancel!",
					i32ModifiedCount);
				INT32 i32Ret = MessageBoxEx(NULL, wsOutput, L"WARNING", MB_OKCANCEL, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
				if (IDOK == i32Ret)
					__SaveCfgData(hWnd);
			}

			// 隐藏窗口
			g_cModifyCfgWnd[_i].HideConfigWnd();
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL __ProcTableKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	size_t stIndex = -1;
	size_t stCount = g_cModifyCfgWnd.size();
	HWND hGreatGrandfatherWnd = GetParent(GetParent(GetParent(GetParent(hWnd))));
	for (size_t _i = 0; _i < stCount; _i++)
	{
		if (hWnd == g_cModifyCfgWnd[_i].GetBaseboardWnd())
			continue;
		if (hGreatGrandfatherWnd != g_cModifyCfgWnd[_i].GetBaseboardWnd())
			continue;

		size_t stSectionCount = g_cModifyCfgWnd[_i].m_vSection.size();
		for (size_t _m = 0; _m < stSectionCount; _m++)
		{
			size_t stKeyCount = g_cModifyCfgWnd[_i].m_vSection[_m].m_vKey.size();
			for (size_t _n = 0; _n < stKeyCount; _n++)
			{
				if (g_cModifyCfgWnd[_i].m_vSection[_m].m_vKey[_n].m_hKeyValue == hWnd)
				{
					if (_n < stKeyCount-1)
						SetFocus(g_cModifyCfgWnd[_i].m_vSection[_m].m_vKey[_n+1].m_hKeyValue);
					else if (_m < stSectionCount-1)
						SetFocus(g_cModifyCfgWnd[_i].m_vSection[_m+1].m_vKey[0].m_hKeyValue);
					else
						SetFocus(g_cModifyCfgWnd[_i].m_vSection[0].m_vKey[0].m_hKeyValue);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}