#include "stdafx.h"
#include <vector>
#include <windows.h>
#include <stdio.h> 
#include <tchar.h>
#include <list>
#include "2.h"
#define	I_MUST_READ_MY_PIPE 11101
#define MAX_BUFFER_SIZE 512
#pragma warning(disable : 4996)
using namespace std;
vector<DWORD> ThreadsId;//список всех id потоков - клиентов
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\MyPipe");
HANDLE hPipeClient;
char fullMesage[255] = "";
DWORD BTR;
HANDLE hPipe = INVALID_HANDLE_VALUE;
HANDLE hThread = NULL;
BOOL bConnected = FALSE;
DWORD dwThreadId = NULL;
MSG msg;
DWORD dwBytesRead;
BOOL bSuccess;
COPYDATASTRUCT cd;
HANDLE hEvent;
HANDLE PipeForSending;
HANDLE CollectAllNamedPipes[100];
DWORD ThreadId;
int i;
HANDLE ReadingPipe;
DWORD WINAPI ClientProc(LPVOID lParam)
{
	char NamePipeClient[100];
	
	TCHAR szBuffer[MAX_BUFFER_SIZE] = { 0 };
	char *UserName1 = (char *)lParam;
	char Reading[100];
	char num[10];
	itoa(i, num, 10);
	strcpy(NamePipeClient, lpszPipename);
	strcat(NamePipeClient, UserName1);
	hPipeClient = CreateNamedPipe(NamePipeClient, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, PIPE_UNLIMITED_INSTANCES, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, 0, NULL);
	strcpy(NamePipeClient1, NamePipeClient);
	strcat(NamePipeClient1,num);
	ReadingPipe = CreateNamedPipe(NamePipeClient1, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, PIPE_UNLIMITED_INSTANCES, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, 0, NULL);
	CollectAllNamedPipes[i] = ReadingPipe;
	i++;
	
	ConnectNamedPipe(hPipeClient, NULL);
	WaitForSingleObject(hEvent, INFINITE);
	bSuccess = ReadFile(hPipeClient, szBuffer, sizeof(szBuffer), &dwBytesRead, NULL);//чтение сообщения из клиента
	strcpy(fullMesage, UserName1);
	strcat(fullMesage, ": ");
	strcat(fullMesage, szBuffer);
	_tprintf(TEXT("bla1\n"));
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
	DWORD mistake;
	for (int c = 0; c <i; c++)
	{
		bSuccess = WriteFile(CollectAllNamedPipes[c], fullMesage, sizeof(fullMesage)+1, &dwBytesRead, NULL);
		mistake = GetLastError();
	}
	DisconnectNamedPipe(hPipeClient);
	return 0;
}
int _tmain(VOID)
{
	 i = 0;
	while (true)
	{
		hEvent = CreateEvent(NULL, FALSE, FALSE, "NamedEvent");
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
		_tprintf(TEXT("[SERVER] Waiting for client connection...\n"));
		bConnected = ConnectNamedPipe(hPipe, NULL);
//		if (bConnected == TRUE)
//		{
			BOOL bSuccess = FALSE;
			DWORD dwBytesRead = 0;
			TCHAR szBuffer[MAX_BUFFER_SIZE] = { 0 };
			char UserName[50] = "";
			char NamePipeClient[100];
			DWORD mist;
			_tprintf(TEXT("[ThreadProc] Created, receiving and processing messages.\n"));		
			if (ReadFile(hPipe, UserName, sizeof(UserName), &dwBytesRead, NULL))
				{
					_tprintf(TEXT("[ThreadProc] Created, receiving and processing messages11111.\n"));
					
					hThread = CreateThread(NULL, 0, ClientProc, UserName, 0, &ThreadId);	
				}
			DisconnectNamedPipe(hPipe);
				_tprintf(TEXT("%s\n"), fullMesage);
//			}//while(старое)
//		}
//		else
//		{
			//The client not connect, so close the pipe.(старое)
			
//		}//(bConnected)(старое)
	}//while(старое)
	
	DisconnectNamedPipe(hPipe);
	_tprintf(TEXT("[ThreadProc] Exitting.1\n"));
	return (DWORD)1;
}