#pragma once
#include <windows.h>
#include <process.h>
#include <stdio.h>

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#ifdef __cplusplus
extern "C" {
#endif
#include "../myStudyBSL/myStudyBSLCommon.h"
#pragma comment(lib, "../../library/myStudyBSL.lib")
#ifdef __cplusplus
}
#endif

#define MAXTHREADCOUNT 30