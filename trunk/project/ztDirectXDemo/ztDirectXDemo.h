#pragma once
#pragma warning(disable: 4996)
/*
DirectX ѧϰ�ʼ�
*/

/*�ļ�����������*/
#include <windows.h>
#include ".\\include\\d3d9.h"
#ifdef WIN32
#pragma comment(lib, ".\\lib\\x86\\d3d9.lib") 
#pragma comment(lib, ".\\lib\\x86\\d3dx9.lib")
#else
#pragma comment(lib, ".\\lib\\x64\\d3d9.lib") 
#pragma comment(lib, ".\\lib\\x64\\d3dx9.lib")
#endif


/*ȫ�ֱ�������*/


/*�ⲿ��������*/


/*�Զ���ṹ��*/
struct stD3DVertex  
{  
	float x;
	float y;
	float z;  
	unsigned long color;  
};


/*�궨��*/
#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE) 


/*��������*/
BOOL RegisterMainClass(wchar_t *wsClsName, HINSTANCE hInstance);				// ע����������
HWND CreateMainWindow(int nCmdShow, HINSTANCE hInstance);						// ����������
BOOL UnRegisterMainClass(wchar_t *wsClsName, HINSTANCE hInstance);				// ������������ע��
BOOL DestroyMainWindow(HWND hWnd);												// ����������
LRESULT WINAPI MainMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);	// ��������Ϣ����

void RenderScene(void);									// ��Ⱦ
BOOL InitializeD3D(HWND hWnd, BOOL bFullScreen);		// ��ʼ��D3D
BOOL DestroyD3D(void);
BOOL InitializeObjects(void);
