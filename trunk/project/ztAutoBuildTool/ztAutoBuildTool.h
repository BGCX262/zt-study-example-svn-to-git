#pragma once
/************************************
	���ܣ��Զ����빤��
	���ߣ���̽
************************************/


/*����ͷ�ļ�*/
#include "ztCommon.h"


/*��������*/
void MainMsgProc(void);
BOOL WriteRunningLog(BOOL bIsWriteFile, wchar_t *wsFormat, ...);
BOOL WINAPI CrashProc(PEXCEPTION_POINTERS Exception);
BOOL SetWndFont(HWND hWnd, wchar_t *pName, INT32 i32Weight, INT32 i32Height);
LRESULT CALLBACK MainWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SubWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL SetCurrentTaskText(wchar_t *pText);					// ���õ�ǰ������ʾ��ǩ
BOOL SetCurrentTaskProgress(INT32 i32Progress);				// ���õ�ǰ�������
BOOL SetTotalTaskProgress(INT32 i32Progress);				// �������������
BOOL ShowLoadScriptWnd(BOOL bIsShow);
unsigned __stdcall RunTaskThreadFunc(void *pParam);			// �����߳�
unsigned __stdcall RunControlThreadFunc(void *pParam);		// �����߳�
BOOL ShowLogControlWnd(void);
BOOL HideLogControlWnd(void);
BOOL CountTaskRunTime(void);