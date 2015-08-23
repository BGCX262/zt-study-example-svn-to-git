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


#define DEFAULTWNDWEITH 800		// ����Ĭ�Ͽ�
#define DEFAULTWNDHEIGHT 480	// ����Ĭ�ϸ�

#define XMOVEWNDSTEP 6			// x�����ƶ�����
#define YMOVEWNDSTEP 4			// y�����ƶ�����