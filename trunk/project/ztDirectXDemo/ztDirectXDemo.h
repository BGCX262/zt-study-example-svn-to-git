#pragma once
#pragma warning(disable: 4996)
/*
DirectX 学习笔记
*/

/*文件包含和引用*/
#include <windows.h>
#include ".\\include\\d3d9.h"
#ifdef WIN32
#pragma comment(lib, ".\\lib\\x86\\d3d9.lib") 
#pragma comment(lib, ".\\lib\\x86\\d3dx9.lib")
#else
#pragma comment(lib, ".\\lib\\x64\\d3d9.lib") 
#pragma comment(lib, ".\\lib\\x64\\d3dx9.lib")
#endif


/*全局变量声明*/


/*外部变量声明*/


/*自定义结构体*/
struct stD3DVertex  
{  
	float x;
	float y;
	float z;  
	unsigned long color;  
};


/*宏定义*/
#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE) 


/*函数声明*/
BOOL RegisterMainClass(wchar_t *wsClsName, HINSTANCE hInstance);				// 注册主窗口类
HWND CreateMainWindow(int nCmdShow, HINSTANCE hInstance);						// 创建主窗口
BOOL UnRegisterMainClass(wchar_t *wsClsName, HINSTANCE hInstance);				// 销毁主窗口类注册
BOOL DestroyMainWindow(HWND hWnd);												// 销毁主窗口
LRESULT WINAPI MainMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);	// 主窗口消息处理

void RenderScene(void);									// 渲染
BOOL InitializeD3D(HWND hWnd, BOOL bFullScreen);		// 初始化D3D
BOOL DestroyD3D(void);
BOOL InitializeObjects(void);
