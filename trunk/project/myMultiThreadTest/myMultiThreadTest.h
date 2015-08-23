#pragma once
#include "common.h"

typedef struct sthreadinfo
{
	void clear(void)
	{
		m_ui32ThreadID = 0;
		m_i32ThreadIndex = -1;
		m_hThreadHandle = NULL;
		m_bIsThreadCreate = FALSE;
		m_i32OutputNum = 0;
	}

	HANDLE m_hThreadHandle;
	UINT32 m_ui32ThreadID;
	INT32 m_i32ThreadIndex;
	BOOL m_bIsThreadCreate;
	INT32 m_i32OutputNum;

}STRU_THREADINFO;

BOOL g_bIsExitMainThread = FALSE;					// ���߳��Ƿ��˳�
INT32 g_i32ThreadCounter = 0;						// �̼߳����������������߳�
CRITICAL_SECTION g_csThreadCtrlLock;				// �߳̿�����
STRU_THREADINFO g_sThreadInfo[MAXTHREADCOUNT];		// �߳���Ϣ

void InitAppData(void);
unsigned __stdcall RunCreateThreadFunc(void *pParam);
unsigned __stdcall RunTestThreadFunc(void *pParam);