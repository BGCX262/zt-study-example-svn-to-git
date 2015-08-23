#include "ztDirectXDemo.h"


/*全局变量定义*/
IDirect3D9 *g_D3D = NULL;
LPDIRECT3DDEVICE9 g_D3DDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_VertexBuffer = NULL; 



/*函数实现*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)		// 应用程序入口
{
	if (RegisterMainClass(L"DirectXDemoClass", hInstance) == FALSE)
		return 1;

	HWND hWnd = CreateMainWindow(nCmdShow, hInstance);
	if (hWnd == NULL)
	{
		UnRegisterMainClass(L"DirectXDemoClass", hInstance);
		return 2;
	}

	InitializeD3D(hWnd, FALSE);

	MSG msg;  
	ZeroMemory(&msg, sizeof(msg));  
	do
	{  
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))  
		{  
			TranslateMessage(&msg);  
			DispatchMessage(&msg);  
		}
		else
			RenderScene();
	}while(msg.message != WM_QUIT);

	DestroyD3D();
	UnRegisterMainClass(L"DirectXDemoClass", hInstance);
	DestroyMainWindow(hWnd);
	return 0;
}

BOOL RegisterMainClass(wchar_t *wsClsName, HINSTANCE hInstance)
{
	DWORD dwLastError = 0;
	WNDCLASSEXW wc;
	memset(&wc, 0, sizeof(WNDCLASSEXW));
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = MainMsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, IDI_WINLOGO);;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = wsClsName;
	wc.hIconSm = NULL;
	if (RegisterClassExW(&wc) == 0)
	{
		dwLastError = GetLastError();
		// log输出
		return FALSE;
	}

	return TRUE;
}

HWND CreateMainWindow(int nCmdShow, HINSTANCE hInstance)
{
	DWORD dwLastError = 0;
	INT32 i32InitWndWidth = 800;	// 初始宽
	INT32 i32InitWndHeight = 480;	// 初始高
	INT32 i32InitWndPosX = (GetSystemMetrics(SM_CXSCREEN) - i32InitWndWidth) / 2;
	INT32 i32InitWndPosY = (GetSystemMetrics(SM_CYSCREEN) - i32InitWndHeight) / 2;

	HWND hWnd = CreateWindow(L"DirectXDemoClass", 
		L"DirectXDemoWnd",
		WS_OVERLAPPEDWINDOW,
		i32InitWndPosX,
		i32InitWndPosY,
		i32InitWndWidth,
		i32InitWndHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (hWnd == NULL)
	{
		dwLastError = GetLastError();
	}
	else
	{
		ShowWindow(hWnd, nCmdShow);  
		UpdateWindow(hWnd); 
	}

	return hWnd;
}

BOOL UnRegisterMainClass(wchar_t *wsClsName, HINSTANCE hInstance)
{
	if (hInstance == NULL)
		return FALSE;
	return UnregisterClass(wsClsName, hInstance);
}

BOOL DestroyMainWindow(HWND hWnd)
{
	return DestroyWindow(hWnd);
}

BOOL InitializeD3D(HWND hWnd, BOOL bFullScreen)
{
	D3DDISPLAYMODE displayMode;  
	g_D3D = Direct3DCreate9(D3D_SDK_VERSION);  
	if(g_D3D == NULL) return FALSE;  

	if(FAILED(g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)))  
		return FALSE; 

	D3DPRESENT_PARAMETERS d3dpp;  
	ZeroMemory(&d3dpp, sizeof(d3dpp));  
	if(bFullScreen)  
	{  
		d3dpp.Windowed = FALSE;  
		d3dpp.BackBufferWidth = 800;  
		d3dpp.BackBufferHeight = 480;  
	}  
	else  
		d3dpp.Windowed = TRUE;  
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;  
	d3dpp.BackBufferFormat = displayMode.Format;  
	
	if(FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_D3DDevice)))  
	{  
		return FALSE;  
	}  

	if(!InitializeObjects()) 
		return FALSE; 

	return TRUE;
}

BOOL DestroyD3D(void)
{
	if(g_D3DDevice != NULL) 
		g_D3DDevice->Release();

	if(g_D3D != NULL) 
		g_D3D->Release();

	if(g_VertexBuffer != NULL) 
		g_VertexBuffer->Release();  
	
	return TRUE;
}

void RenderScene(void)
{
	g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);	// Clear the backbuffer.  
	g_D3DDevice->BeginScene();														// Begin the scene. Start rendering.
	  
	g_D3DDevice->SetStreamSource(0, g_VertexBuffer, 0, sizeof(stD3DVertex));		// Render object.
	g_D3DDevice->SetFVF(D3DFVF_VERTEX);  
	g_D3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, 2);  

	g_D3DDevice->EndScene();														// End the scene. Stop rendering.  
	g_D3DDevice->Present(NULL, NULL, NULL, NULL);									// Display the scene.
}

BOOL InitializeObjects(void)
{
	unsigned long col = D3DCOLOR_XRGB(255, 255, 255);  
	stD3DVertex objData[] =  
	{  
		{420.0f, 150.0f, 0.5f, col},  
		{420.0f, 350.0f, 0.5f, col},  
		{220.0f, 150.0f, 0.5f, col},  
		{220.0f, 350.0f, 0.5f, col},  
	};  
	 
	if(FAILED(g_D3DDevice->CreateVertexBuffer(sizeof(objData), 0, D3DFVF_VERTEX, D3DPOOL_DEFAULT, &g_VertexBuffer, NULL)))	// Create the vertex buffer. 
		return FALSE;  
	
	void *ptr;  
	if(FAILED(g_VertexBuffer->Lock(0, sizeof(objData), (void**)&ptr, 0))) 
		return FALSE;  
	memcpy(ptr, objData, sizeof(objData));  
	g_VertexBuffer->Unlock();  
	return true; 
}