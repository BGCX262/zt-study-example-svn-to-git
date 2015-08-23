#include "mySocketClient.h"


/*全局变量*/
LPDIRECT3D9 g_D3D = NULL;						// Direct3D object.
LPDIRECT3DDEVICE9 g_D3DDevice = NULL;			// Direct3D device.
LPDIRECT3DVERTEXBUFFER9 g_VertexBuffer = NULL;	// Vertex buffer to hold the geometry.


/*函数实现*/
BOOL InitializeD3D(HWND hWnd, BOOL bFullscreen)
{
	D3DDISPLAYMODE displayMode;

	// Create the D3D object.
	g_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_D3D == NULL) 
		return FALSE;

	// Get the desktop display mode.
	if(FAILED(g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode))) 
		return FALSE;

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	if(bFullscreen)
	{
		d3dpp.Windowed = FALSE;
		d3dpp.BackBufferWidth = g_i32MainWndWidth;
		d3dpp.BackBufferHeight = g_i32MainWndHeight;
	}
	else
		d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = displayMode.Format;

	// Create the D3DDevice
	if(FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_D3DDevice))) 
		return FALSE;

	// Initialize any objects we will be displaying.
	if(!InitializeObjects()) 
		return FALSE;

	if (g_hLogicThread != NULL && g_hLogicThread != INVALID_HANDLE_VALUE)
		ResumeThread(g_hLogicThread);
	if (g_hRenderThread != NULL && g_hRenderThread != INVALID_HANDLE_VALUE)
		ResumeThread(g_hRenderThread);
	return TRUE;
}

BOOL InitializeObjects(void)
{
	unsigned long col = D3DCOLOR_XRGB(255, 0, 0);

	// Fill in our structure to draw an object.
	// x, y, z, rhw, color.
	STRUD3DVERTEX objData[] =
	{
		{5.0f, 5.0f, 0.5f, 1.0f, col,},
		{5.0f, 105.0f, 0.5f, 1.0f, col,},
		{105.0f, 5.0f, 0.5f, 1.0f, col,},
		{105.0f, 105.0f, 0.5f, 1.0f, col,},
	};

	// Create the vertex buffer.
	if(FAILED(g_D3DDevice->CreateVertexBuffer(sizeof(objData), 0, D3DFVF_VERTEX, D3DPOOL_DEFAULT, &g_VertexBuffer, NULL))) 
		return FALSE;

	// Fill the vertex buffer.
	void *ptr = NULL;

	if(FAILED(g_VertexBuffer->Lock(0, sizeof(objData), (void**)&ptr, 0))) 
		return FALSE;

	memcpy(ptr, objData, sizeof(objData));

	g_VertexBuffer->Unlock();

	return TRUE;
}

void RenderScene(void)
{
	srand(GetTickCount());
	unsigned long col = D3DCOLOR_XRGB(rand() % 255, rand() % 255, rand() % 255);
	STRUD3DVERTEX objData[] =
	{
		{(float)(rand() % g_i32MainWndWidth), (float)(rand() % g_i32MainWndHeight), 0.5f, 1.0f, col,},
		{(float)(rand() % g_i32MainWndWidth), (float)(rand() % g_i32MainWndHeight), 0.5f, 1.0f, col,},
		{(float)(rand() % g_i32MainWndWidth), (float)(rand() % g_i32MainWndHeight), 0.5f, 1.0f, col,},
		{(float)(rand() % g_i32MainWndWidth), (float)(rand() % g_i32MainWndHeight), 0.5f, 1.0f, col,},
	};
	void *ptr = NULL;
	if(FAILED(g_VertexBuffer->Lock(0, sizeof(objData), (void**)&ptr, 0))) 
		return;
	memcpy(ptr, objData, sizeof(objData));
	g_VertexBuffer->Unlock();

	// Clear the backbuffer.
	g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	// Begin the scene.  Start rendering.
	g_D3DDevice->BeginScene();

	// Render object.
	g_D3DDevice->SetStreamSource(0, g_VertexBuffer, 0, sizeof(STRUD3DVERTEX));
	g_D3DDevice->SetFVF(D3DFVF_VERTEX);
	g_D3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 3);

	// End the scene.  Stop rendering.
	g_D3DDevice->EndScene();

	// Display the scene.
	g_D3DDevice->Present(NULL, NULL, NULL, NULL);
}

void DestroyD3DResouse(void)
{
	if(g_D3DDevice != NULL) 
		g_D3DDevice->Release();
	if(g_D3D != NULL) 
		g_D3D->Release();
	if(g_VertexBuffer != NULL) 
		g_VertexBuffer->Release();

	g_D3DDevice = NULL;
	g_D3D = NULL;
	g_VertexBuffer = NULL;
}