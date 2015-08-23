#include "ztDirectXDemo.h"


LRESULT WINAPI MainMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)  
{  
	switch(msg)  
	{  
	case WM_DESTROY:  
		PostQuitMessage(0);  
		return 0;  

	case WM_KEYUP:  
		if(wParam == VK_ESCAPE) 
			PostQuitMessage(0);  
		break; 
	}  

	return DefWindowProc(hWnd, msg, wParam, lParam);  
}