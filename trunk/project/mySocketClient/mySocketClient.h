#pragma once

/*文件包含*/
#include "common.h"


/*宏定义*/
#define DEFAULTWNDWEITH 400
#define DEFAULTWNDHEIGHT 225
#define D3DFVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)		// Our custom FVF, which describes our custom vertex structure.


/*结构体定义*/
typedef struct struD3DVertex	// A structure for our custom vertex type
{
	float x, y, z, rhw;
	unsigned long color;
}STRUD3DVERTEX, *PSTRUD3DVERTEX;


/*外部变量*/
extern LPDIRECT3D9 g_D3D;
extern LPDIRECT3DDEVICE9 g_D3DDevice;
extern LPDIRECT3DVERTEXBUFFER9 g_VertexBuffer;

extern HWND g_hThisWndHandle;
extern wchar_t g_wsClassName[128];
extern HINSTANCE g_hThisAppInstance;

extern INT32 g_i32MainWndWidth;
extern INT32 g_i32MainWndHeight;

extern BOOL g_bIsMainThreadExit;
extern HANDLE g_hLogicThread;
extern HANDLE g_hRenderThread;


/*函数声明*/
BOOL WINAPI CrashProc(PEXCEPTION_POINTERS Exception);
LRESULT CALLBACK MainWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL InitAppData(HINSTANCE hInstance);
void DestroyAppData(void);
INT32 ProcCmdLine(LPSTR lpCmdLine);

BOOL RegisterMyClass(void);
BOOL CreateMyWindow(INT32 cmdShow);
void MainMsgProc(void);

BOOL InitializeD3D(HWND hWnd, BOOL bFullscreen);
BOOL InitializeObjects(void);
void RenderScene(void);
void DestroyD3DResouse(void);

unsigned __stdcall LogicThread(void *pParam);
unsigned __stdcall RenderThread(void *pParam);