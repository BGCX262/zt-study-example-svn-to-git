#include "ztWorkToolOutputWnd.h"

#define BUFSIZE (1024*20)


int main(int argc, char* argv[])
{
	size_t ztNameLen = 0;
	char szPipeName[256] = "";
	char szLogFileName[256] = "";
	GetModuleFileNameEx(GetCurrentProcess(), NULL, szLogFileName, sizeof(szLogFileName));
	ztNameLen = strlen(szLogFileName);
	szLogFileName[ztNameLen-3] = 'i';
	szLogFileName[ztNameLen-2] = 'n';
	szLogFileName[ztNameLen-1] = 'i';
	GetPrivateProfileString("NamedPipe", "OutputWnd", "ERROR", szPipeName, sizeof(szPipeName), szLogFileName);
	if (strcmp(szPipeName, "ERROR") == 0)
	{
		char szInput[128] = "";
		printf("Config file error!");
		scanf("%s", szInput);
		return 1;
	}
	HANDLE hPipe = CreateNamedPipe(
		szPipeName, 
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT, 
		PIPE_UNLIMITED_INSTANCES,
		BUFSIZE,
		BUFSIZE,
		5000, 
		NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		char szInput[128] = "";
		printf("Create pipe failed! Last error is %u", GetLastError());
		scanf("%s", szInput);
		return 1;
	}

	printf("Wait connect...\n");
	BOOL bConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
	if (bConnected)
	{
		printf("Pipe connected\n");
		while (TRUE)
		{
			DWORD dwReadSize = 0;
			char szBuf[BUFSIZE] = "";
			BOOL bSuccess = ReadFile(hPipe, szBuf, BUFSIZE, &dwReadSize, NULL);  

			if (bSuccess == FALSE || dwReadSize == 0)
			{
				printf("Pipe disconnect!\n");
				break;
			}

			if (stricmp(szBuf, "exit") == 0)
				break;

			printf(szBuf);
		}
	}

	FlushFileBuffers(hPipe); 
	DisconnectNamedPipe(hPipe); 
	CloseHandle(hPipe);
	return 0;
}