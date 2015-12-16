#include "stdafx.h"
#include <vector>
#include <windows.h>
#include <stdio.h> 
#include <tchar.h>
#define MAX_BUFFER_SIZE 512
#pragma warning(disable : 4996)
using namespace std;
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\MyPipe");
HANDLE CollectAllNamedPipes[100];
int i;
DWORD WINAPI ClientProc(LPVOID lParam)
{
	HANDLE hPipeClient=INVALID_HANDLE_VALUE;
	HANDLE ReadingPipeServer=INVALID_HANDLE_VALUE;
	DWORD dwBytesRead=0;
	BOOL bSuccess = FALSE;
	char NamePipeClientIn[100];
	char NamePipeClientOut[100];
	char szBuffer[100] = "";
	char *UserName1 = (char *)lParam;
	while (true)
	{
		strcpy(NamePipeClientIn, lpszPipename); //формируем имя канала для получения сообщений от клиента(считываем из него)
		strcat(NamePipeClientIn, UserName1);
		strcat(NamePipeClientIn, "_in");
		if (hPipeClient == INVALID_HANDLE_VALUE)
			hPipeClient = CreateNamedPipe(NamePipeClientIn, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, 0, NULL);
		strcpy(NamePipeClientOut, lpszPipename);//формируем имя канала для отправки сообщений клиенту(в него записываем)
		strcat(NamePipeClientOut, UserName1);
		strcat(NamePipeClientOut, "_out");
		if (ReadingPipeServer == INVALID_HANDLE_VALUE) // если канала еще нет, условие ставится чтобы канал не перезаписывался(у нас бесконечный while)
		{
			ReadingPipeServer = CreateNamedPipe(NamePipeClientOut, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, 0, NULL);
			CollectAllNamedPipes[i] = ReadingPipeServer;
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
				_tprintf(TEXT("[ClientProc] Client disconnected.\n"));
				break;
			}
			else
			{
				_tprintf(TEXT("[ClientProc] ReadFile failed, Error %ld.\n"),
					GetLastError());
				break;
			}
		}
		for (int c = 0; c < i; c++)//идём по списку и отправляем
		{
			bSuccess = WriteFile(CollectAllNamedPipes[c], szBuffer, sizeof(szBuffer)+1, &dwBytesRead, NULL);
			if (!bSuccess)
			{
				_tprintf(TEXT("[ClientProc] WriteFile failed, GLE=%d.\n"), GetLastError());
				break;
			}
		}
	}
	FlushFileBuffers(hPipeClient);
	DisconnectNamedPipe(hPipeClient);
	CloseHandle(hPipeClient);
	FlushFileBuffers(ReadingPipeServer);
	DisconnectNamedPipe(ReadingPipeServer);
	CloseHandle(ReadingPipeServer);
	printf("ClientProc exitting.\n");
	return 1;
}
int _tmain(VOID)
{
	BOOL bConnected = FALSE;
	DWORD ThreadId=0;
	HANDLE hPipe=INVALID_HANDLE_VALUE;
	HANDLE hThread = NULL;
	DWORD dwBytesRead = 0;
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
		_tprintf(TEXT("[SERVER] Waiting for client connection to the Server pipe...\n"));
		bConnected = ConnectNamedPipe(hPipe, NULL);
		if (bConnected)
		{
			char UserName[50] = "";
			_tprintf(TEXT("[SERVER] Server pipe is ready for reading messages.\n"));
			if (ReadFile(hPipe, UserName, sizeof(UserName), &dwBytesRead, NULL))
			{
				hThread = CreateThread(NULL, 0, ClientProc, UserName, 0, &ThreadId);
				_tprintf(TEXT("[SERVER] Thread for the client %s created.\n"), UserName);
				if (hThread == NULL)
				{
					_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
					return -1;
				}
				else CloseHandle(hThread);
			}
			else
				// The client could not connect, so close the pipe. 
				CloseHandle(hPipe);
		}
	}	
}