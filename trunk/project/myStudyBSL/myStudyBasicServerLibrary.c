#include "myStudyBasicServerLibrary.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		printf("Process attach.\n");
		break;

	case DLL_PROCESS_DETACH:
		printf("Process detach.\n");
		break;

	case DLL_THREAD_ATTACH:
		printf("Thread attach.\n");
		break;

	case DLL_THREAD_DETACH:
		printf("Thread detach.\n");
		break;
	}

	return TRUE;
}