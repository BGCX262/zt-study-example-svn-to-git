/*预处理*/
#pragma once
#pragma warning(disable: 4996)



/*接口*/
BOOL __stdcall InitLogAndCfgWnd(HWND hFatherWnd);							// 初始化Log和Cfg窗口
BOOL __stdcall ShowCfgWnd(void);											// 显示Config窗口
BOOL __stdcall HideCfgWnd(void);											// 隐藏Config窗口
BOOL __stdcall ShowLineLossWnd(void);										// 显示LineLoss窗口
BOOL __stdcall HideLineLossWnd(void);										// 隐藏LineLoss窗口
BOOL __stdcall ShowLimitsWnd(void);											// 显示Limits窗口
BOOL __stdcall HideLimitsWnd(void);											// 影藏Limits窗口
BOOL __stdcall DestroyLogAndCfgRes(void);									// 销毁窗口资源
BOOL __stdcall AddLogToWndW(wchar_t *wsLog, double dbTimeMS);				// 添加Log到窗口(Unicode)
BOOL __stdcall AddLogToWndA(char *szLog, double dbTimeMS);					// 添加Log到窗口(ASCII)
#ifdef UNICODE
#define AddLogToWnd AddLogToWndW
#else
#define AddLogToWnd AddLogToWndA
#endif
BOOL __stdcall ClearLogWnd(void);											// 清空log
BOOL __stdcall IsSaveLogToFile(BOOL bIsSave);								// 是否保存log到文件 