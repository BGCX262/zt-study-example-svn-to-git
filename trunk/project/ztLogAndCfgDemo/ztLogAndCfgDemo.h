/*Ԥ����*/
#pragma once
#pragma warning(disable: 4996)



/*�ӿ�*/
BOOL __stdcall InitLogAndCfgWnd(HWND hFatherWnd);							// ��ʼ��Log��Cfg����
BOOL __stdcall ShowCfgWnd(void);											// ��ʾConfig����
BOOL __stdcall HideCfgWnd(void);											// ����Config����
BOOL __stdcall ShowLineLossWnd(void);										// ��ʾLineLoss����
BOOL __stdcall HideLineLossWnd(void);										// ����LineLoss����
BOOL __stdcall ShowLimitsWnd(void);											// ��ʾLimits����
BOOL __stdcall HideLimitsWnd(void);											// Ӱ��Limits����
BOOL __stdcall DestroyLogAndCfgRes(void);									// ���ٴ�����Դ
BOOL __stdcall AddLogToWndW(wchar_t *wsLog, double dbTimeMS);				// ���Log������(Unicode)
BOOL __stdcall AddLogToWndA(char *szLog, double dbTimeMS);					// ���Log������(ASCII)
#ifdef UNICODE
#define AddLogToWnd AddLogToWndW
#else
#define AddLogToWnd AddLogToWndA
#endif
BOOL __stdcall ClearLogWnd(void);											// ���log
BOOL __stdcall IsSaveLogToFile(BOOL bIsSave);								// �Ƿ񱣴�log���ļ� 