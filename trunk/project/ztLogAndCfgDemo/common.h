/*Ԥ����*/
#pragma once
#pragma warning(disable: 4996)


/*����ͷ�ļ���������*/
#include <windows.h>
#include <process.h>
#include <Commctrl.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <vector>
using namespace std;


/*�Զ���*/
typedef struct struKeyInfo
{
	HWND m_hKey;
	HWND m_hKeyValue;
	HWND m_hFrame;
	wchar_t m_wsKey[128];
	wchar_t m_wsKeyValue[128];
	WNDPROC m_pfnCallback;
}STRUKEYINFO, *PSTRUKEYINFO;

typedef struct struSectionInfo
{
	HWND m_hSectionFrame;
	HWND m_hSectionCaption;
	wchar_t m_wsSectionName[128];
	INT32 m_i32SectionWidth;
	INT32 m_i32SectionHeight;
	INT32 m_i32SectionPosX;
	INT32 m_i32SectionPosY;
	vector<STRUKEYINFO> m_vKey;
}STRUSECTIONINFO, *PSTRUSECTIONINFO;


/*�ඨ��*/
class CModifyConfig
{
public:
	WNDPROC m_lpfnDefaultMsgProc;
	vector<STRUSECTIONINFO> m_vSection;

private:
	HWND m_hBaseboard;							// �װ崰�ھ��
	HWND m_hShowBaseboard;						// ��ʾ�壬�����϶�ʹ��
	
	wchar_t m_wsCfgFileName[256];
	INT32 m_i32CurWndWidth;						// ��ǰ���ڵĿ�
	INT32 m_i32CurWndHeight;					// ��ǰ���ڵĸ�

	INT32 m_i32CurContentHeight;				// ��ǰ���ݵ�ʵ�ʸ�
	INT32 m_i32CurShowPosY;						// ��ǰ��ʾ���ݵ�Y����

public:
	CModifyConfig();
	~CModifyConfig();

public:
	BOOL InitSection(HWND &hWnd, wchar_t *wsCfgFileName, wchar_t* szSectionName[], INT32 i32SectionCount);
	BOOL DestroySection(void);
	BOOL AddItem(wchar_t* wsSectionName, wchar_t* wsKeyName);
	BOOL DelItem(void);
	BOOL ShowConfigWnd(void);
	BOOL HideConfigWnd(void);
	void UpdateClientShow(INT32 i32ShowPos);

	HWND GetBaseboardWnd(void);
	HWND GetShowBaseboardWnd(void);

	BOOL SaveCfg(void);
	INT32 GetUnsaveCount(void);

	LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};


/*�ⲿ����*/
extern int g_iStatusInitShow;
extern HWND g_hMainWnd;
extern HWND g_hLogWnd;
extern HWND g_hFatherWnd;
extern HWND g_hCfgWnd;
extern HWND g_hLineLossWnd;
extern HWND g_hLimitsWnd;
extern HINSTANCE g_hThisInstance;
extern wchar_t g_wsWndClsName[64];
extern wchar_t g_wsThisCfgName[256];
extern wchar_t g_wsThisCfgFullPath[256];
extern wchar_t g_wsThisInstancePath[256];
extern BOOL g_bIsMainThreadExit;
extern HANDLE g_hLogicThread;
extern UINT32 g_ui32LogIndex;
extern BOOL g_bIsSaveLogToFile;
extern vector<HFONT> g_vFontRes;
extern WNDPROC g_lpfnLogWndProc;
//extern CModifyConfig g_cConfigWnd;
//extern CModifyConfig g_cLineLossTableWnd;
//extern CModifyConfig g_cLimitsWnd;
extern vector<CModifyConfig> g_cModifyCfgWnd;


/*��������*/
BOOL InitAppData(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK MainMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WidgetsMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL RegisterDemoClass(void);
BOOL UnRegisterDemoClass();
BOOL CreateDemoWindow();
BOOL DestroyDemoWindow(void);
void MainMsgPeek(void);
BOOL SetWndFont(HWND hWnd, wchar_t *pName, INT32 i32Weight, INT32 i32Height, INT32 i32Width);

BOOL CreateMainWidgets(void);
BOOL CreateLogWidgets(void);
BOOL CreateCfgWidgets(void);

unsigned __stdcall ThreadLogic(void *pPara);