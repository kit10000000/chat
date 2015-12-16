#include "stdafx.h"
#include <vector>
#include <windows.h>
#include <stdio.h> 
#include <tchar.h>
#include <list>
#include "2.h"
#define MAX_BUFFER_SIZE 512
#pragma warning(disable : 4996)
using namespace std;
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\MyPipe");

DWORD BTR;
HANDLE hPipe;
HANDLE hThread = NULL;
BOOL bConnected = FALSE;
DWORD dwThreadId = NULL;
DWORD dwBytesRead;
COPYDATASTRUCT cd;
HANDLE hEvent;
HANDLE CollectAllNamedPipes[100];
DWORD ThreadId;
int i;

DWORD WINAPI ClientProc(LPVOID lParam)
{
	HANDLE hPipeClient=INVALID_HANDLE_VALUE;
	HANDLE ReadingPipe=INVALID_HANDLE_VALUE;
	while (true)
	{

		DWORD mistake;
		BOOL bSuccess;
		char NamePipeClientIn[100];
		char NamePipeClientOut[100];
		char szBuffer[100]="";
		char *UserName1 = (char *)lParam;
		hEvent = CreateEvent(NULL, FALSE, FALSE, "NamedEvent");
		strcpy(NamePipeClientIn, lpszPipename);
		strcat(NamePipeClientIn, UserName1);
		strcat(NamePipeClientIn, "_in");
		if (hPipeClient==INVALID_HANDLE_VALUE)
			hPipeClient = CreateNamedPipe(NamePipeClientIn, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, 0, NULL);
		strcpy(NamePipeClientOut, lpszPipename);
		strcat(NamePipeClientOut, UserName1);
		strcat(NamePipeClientOut, "_out");
		if (ReadingPipe == INVALID_HANDLE_VALUE)
		{
			ReadingPipe = CreateNamedPipe(NamePipeClientOut, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, 0, NULL);
			mistake = GetLastError();
			CollectAllNamedPipes[i] = ReadingPipe;
			i++;
		}
		bSuccess = ConnectNamedPipe(hPipeClient, NULL);
		if (bSuccess)
			_tprintf(TEXT("[ClientProc] Client is connected and ready for reading.\n"));
		bSuccess = ReadFile(hPipeClient, szBuffer, sizeof(szBuffer), &dwBytesRead, NULL);//чтение сообщения из клиента
		if ((FALSE == bSuccess) || (NULL == dwBytesRead))
		{
			if (ERROR_BROKEN_PIPE == GetLastError())
			{
				_tprintf(TEXT("[ThreadProc] Client disconnected.\n"));
			}
			else
			{
				_tprintf(TEXT("[ThreadProc] ReadFile failed, Error %ld.\n"),
					GetLastError());
			}
		}
		for (int c = 0; c < i; c++)
		{
			bSuccess = WriteFile(CollectAllNamedPipes[c], szBuffer, sizeof(szBuffer)+1, &dwBytesRead, NULL);
			if (bSuccess)
				_tprintf(TEXT("[ClientProc] Message was sent to the reading channel %d"), c);
		}

	}
}
int _tmain(VOID)
{
	 i = 0;
	while (true)
	{
		
		hPipe = CreateNamedPipe(lpszPipename, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES | PIPE_NOWAIT, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, 0,
			NULL);
		if (INVALID_HANDLE_VALUE == hPipe)
		{
			switch (GetLastError())
			{
			case ERROR_PIPE_BUSY:
				WaitNamedPipe(lpszPipename, 5000);
				continue;
			default:
				_tprintf(TEXT("[SERVER] CreateNamedPipe failed, Error %ld\n"),
					GetLastError());
				return -1;
			}
		}
		_tprintf(TEXT("[main] Waiting for client connection to the Server pipe...\n"));
		bConnected = ConnectNamedPipe(hPipe, NULL);
			DWORD dwBytesRead = 0;
			char UserName[50] = "";
			_tprintf(TEXT("[main] Server pipe is ready for reading messages.\n"));		
			if (ReadFile(hPipe, UserName, sizeof(UserName), &dwBytesRead, NULL))
				{
					hThread = CreateThread(NULL, 0, ClientProc, UserName, 0, &ThreadId);	
					_tprintf(TEXT("[main] Thread for the client %s created."),UserName);
				}
			DisconnectNamedPipe(hPipe);
	}//while(старое)
	
	DisconnectNamedPipe(hPipe);
	_tprintf(TEXT("[ThreadProc] Exitting.1\n"));
	return 1;
}