#pragma once
#include <windows.h>
#include <string.h>
#include <locale.h>
#include <stdio.h>

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#include "CrashRpt.h"
#pragma comment(lib, "..\\..\\library\\CrashRpt.lib")

#ifdef __cplusplus
extern "C" {
#endif
#include "../myStudyBSL/myStudyBasicServerLibrary.h"
#pragma comment(lib, "../../library/myStudyBSL.lib")
#ifdef __cplusplus
}
#endif

#include "XLError.h"
#include "XLDownload.h"
#pragma comment(lib, "../../library/XLDownload.lib")


#define DEFAULTWNDWEITH 800		// 窗口默认宽
#define DEFAULTWNDHEIGHT 480	// 窗口默认高

#define XMOVEWNDSTEP 6			// x方向移动步长
#define YMOVEWNDSTEP 4			// y方向移动步长