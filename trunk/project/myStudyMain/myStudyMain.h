#pragma once
#include "myStudyCommon.h"

static INT32 ProcCmdLine(LPSTR lpCmdLine);		// ����������
static BOOL RegisterMyClass(void);				// ������ע��
static BOOL CreateMyWindow(INT32 cmdShow);		// ���ڴ���
static BOOL DestroyMyWindow(void);				// ��������
LRESULT CALLBACK MainWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);		// ��������Ϣ�ص�
LRESULT CALLBACK InputCtrlMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI CrashProc(PEXCEPTION_POINTERS Exception);

static void InitAppData(void);
static void ShowMessageBox(wchar_t *pCaption, wchar_t wsTextFormat, ...);			// �˺���δʵ��
void OnPressUpKey(HWND hWnd, LPARAM lParam);				// �ϼ�����
void OnPressDownKey(HWND hWnd, LPARAM lParam);				// �¼�����
void OnPressLeftKey(HWND hWnd, LPARAM lParam);				// �������
void OnPressRightKey(HWND hWnd, LPARAM lParam);				// �Ҽ�����

extern HWND g_hThisWndHandle;					// �����ھ��
extern HINSTANCE g_hThisInstance;				// ��ʵ�����
extern wchar_t g_wsClassName[128];				// ��������
extern WNDPROC g_lpfnInputWndBackup;			// ���봰��ԭ�ص����� 