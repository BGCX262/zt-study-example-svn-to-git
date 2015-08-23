#include "myStudyBasicServerLibrary.h"

BOOL __stdcall GetProcessNameByPIDA(__out char* szProcessName, __in UINT32 dwPID)
{
#ifdef WIN32
	char szName[MAX_PATH] = "";
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
	if (hProcess == NULL)
		return FALSE;
	if (GetModuleFileNameEx(hProcess, NULL, szName, MAX_PATH) == 0)
		return FALSE;
	strcpy(szProcessName, szName);
	return TRUE;
#else
	return FALSE;
#endif
}

BOOL __stdcall GetProcessNameByPIDW(__out wchar_t* wsProcessName, __in UINT32 dwPID)
{
#ifdef WIN32
	wchar_t wsName[MAX_PATH] = L"";
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
	if (hProcess == NULL)
		return FALSE;
	if (GetModuleFileNameEx(hProcess, NULL, wsName, MAX_PATH) == 0)
		return FALSE;
	wcscpy(wsProcessName, wsName);
	return TRUE;
#else
	return FALSE;
#endif
}