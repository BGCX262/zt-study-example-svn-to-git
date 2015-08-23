/*++

Copyright (c) 2009 xuyibo.org

Module Name:

    CrashRpt.h

Abstract:

    This module contains export interfaces of CrashRpt.dll

Author:

    xuyibo (xuyibo@live.cn) 2009-06-28

--*/

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __STATIC_CRASHRPT__
#define DECLAPI
#else
#define DECLAPI DECLSPEC_IMPORT
#endif
    
// Misc crash dump information mask
#define CRASHRPT_ERROR      0x00000001
#define CRASHRPT_SYSTEM     0x00000002
#define CRASHRPT_SOFTWARE   0x00000004
#define CRASHRPT_HARDWARE   0x00000008
#define CRASHRPT_PROCESS    0x00000010
#define CRASHRPT_SERVICE    0x00000020
#define CRASHRPT_DRIVER     0x00000040
#define CRASHRPT_STARTUP    0x00000080
#define CRASHRPT_HARDDISK   0x00000100
#define CRASHRPT_NETCARD    0x00000200
#define CRASHRPT_PORT       0x00000400
#define CRASHRPT_WINSOCK    0x00000800
#define CRASHRPT_IEPLUGIN   0x00001000

// Generate minidump
#define CRASHRPT_MINIDUMP   0x10000000

// Genearete all information
#define CRASHRPT_ALL        0xFFFFFFFF

DECLAPI void WINAPI InitializeCrashRpt();

typedef BOOL (WINAPI* CRASHRPTPROC)(PEXCEPTION_POINTERS Exception);
DECLAPI void WINAPI InitializeCrashRptEx(CRASHRPTPROC CallBack);

#ifdef UNICODE
#define GenerateCrashRpt GenerateCrashRptW
#else
#define GenerateCrashRpt GenerateCrashRptA
#endif
DECLAPI LONG WINAPI GenerateCrashRptA(PEXCEPTION_POINTERS Exception, LPCSTR XmlFileName, DWORD Mask);
DECLAPI LONG WINAPI GenerateCrashRptW(PEXCEPTION_POINTERS Exception, LPCWSTR XmlFileName, DWORD Mask);

#ifdef UNICODE
#define GenerateMiniDump GenerateMiniDumpW
#else
#define GenerateMiniDump GenerateMiniDumpA
#endif
DECLAPI BOOL WINAPI GenerateMiniDumpA(PEXCEPTION_POINTERS Exception, LPCSTR MiniDumpFileName);
DECLAPI BOOL WINAPI GenerateMiniDumpW(PEXCEPTION_POINTERS Exception, LPCWSTR MiniDumpFileName);

#ifdef UNICODE
#define ShowCrashRpt ShowCrashRptW
#else
#define ShowCrashRpt ShowCrashRptA
#endif
DECLAPI BOOL WINAPI ShowCrashRptA(HWND hParent, LPCSTR XmlFileName);
DECLAPI BOOL WINAPI ShowCrashRptW(HWND hParent, LPCWSTR XmlFileName);

#define SendMail SendMailA
DECLAPI BOOL WINAPI SendMailA(HWND hParent, LPSTR AttachFile, LPSTR Subject, LPSTR MailList);

#ifdef __cplusplus
}
#endif