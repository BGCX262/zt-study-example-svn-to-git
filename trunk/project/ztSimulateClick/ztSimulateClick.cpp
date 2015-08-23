#pragma warning(disable: 4996)

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

BOOL SimulateLeftMouseClick(LONG lPosX, LONG lPosY);

int main(int argc, char *argv[])  
{
	printf("Welcome!\n");
	INT32 i32SpaceTime = 0;
	INT32 i32ClickTimes = 0;
	do
	{
		printf("Please enter space time(s) (10<x<1000):");
		scanf("%d", &i32SpaceTime);
	}while (i32SpaceTime < 10 || i32SpaceTime > 999);
	printf("Space time is %ds!\n\nWaiting...\n", i32SpaceTime);
	Sleep(5000);

	SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	i32ClickTimes++;
	SimulateLeftMouseClick(1284, 484);

	while (TRUE)
	{
		INT32 i32BeginTime = GetTickCount();
		while ((INT32)(GetTickCount() - i32BeginTime) < i32SpaceTime * 1000)
		{
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
			printf("Click times: %d  RemainderTime: %-5.1fs    ", i32ClickTimes, i32SpaceTime - (double)(GetTickCount() - i32BeginTime)/1000.0);
			Sleep(50);
		}

		i32ClickTimes++;
		i32BeginTime = GetTickCount();
		SimulateLeftMouseClick(1284, 484);
	}
}

// 模拟鼠标左键单击操作 add by zhangtan @2013-02-20
BOOL SimulateLeftMouseClick(LONG lPosX, LONG lPosY)
{
	INPUT struInput;
	
	memset(&struInput, 0, sizeof(INPUT));
	struInput.type = INPUT_MOUSE;					// 移动鼠标
	struInput.mi.dwFlags = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
	struInput.mi.dx = lPosX * 65535 / GetSystemMetrics(SM_CXSCREEN);
	struInput.mi.dy = lPosY * 65535 / GetSystemMetrics(SM_CYSCREEN);
	SendInput(1, &struInput, sizeof(INPUT));
	Sleep(10);
	
	memset(&struInput, 0, sizeof(INPUT));
	struInput.type = INPUT_MOUSE;
	struInput.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;	// 鼠标按下
	SendInput(1, &struInput, sizeof(INPUT));
	Sleep(10);

	memset(&struInput, 0, sizeof(INPUT));
	struInput.type = INPUT_MOUSE;
	struInput.mi.dwFlags = MOUSEEVENTF_LEFTUP;		// 鼠标抬起
	SendInput(1, &struInput, sizeof(INPUT));

	return TRUE;
}