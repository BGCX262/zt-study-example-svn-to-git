#pragma once
#include <windows.h>
#include <psapi.h>
#include <Tlhelp32.h>
#include <stdio.h>
#include <process.h>

#pragma comment(lib, "psapi.lib")

#pragma warning(disable: 4996)


/*��������*/
BOOL WriteAppLog(char *szLog);				// д��־
BOOL DestroyRes(void);						// ��Դ����
BOOL AppRunning(void);						// ��ѭ�������к���
BOOL AnalyseCmd(char *szCmd);				// �������
BOOL IsFileExist(char *szFileName);			// ����ļ��Ƿ����
BOOL InitData(int argc, char* argv[]);		// ���ݳ�ʼ��
BOOL SendMsgToOutputWnd(char *szBuf�� , BOOL bShowTime);	// ������Ϣ���������
unsigned __stdcall ExecuteLogicThread(void *pAnything);		// �߼��߳�
BOOL ExitProcessTree(BOOL bIsExitItself, DWORD dwThisProcessID);	// ����������
BOOL GetFileNameAndPathByFullName(char __out *pFileName, char __out *pFilePath, char __in *pFileFullName);


/*�ⲿ��������*/
extern DWORD g_dwCurRunPID;
extern INT32 g_i32RunAppTime;
extern CHAR g_szAppCfgName[256];				
extern CHAR g_szAppLogName[256];				
extern BOOL g_bIsExitThisProcess;
extern INT32 g_i32ThreadCounter;
extern char g_szRunAppName[256];

extern HANDLE g_hCanCreateProcessEvt;

extern CRITICAL_SECTION g_csAppLogMutex;		
extern CRITICAL_SECTION g_csAppCommonMutex;		
