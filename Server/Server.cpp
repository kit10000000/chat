#include "stdafx.h"
#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#pragma warning(disable : 4996)
DWORD WINAPI ThreadProc(LPVOID);
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\MyPipe");
HANDLE hMutex;
CHAR myMutex[] =
"MutexName";
int _tmain(VOID)
{
	const int MAX_BUFFER_SIZE = 512;
	
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	HANDLE hThread = NULL;
	BOOL bConnected = FALSE;
	DWORD dwThreadId = NULL;
	while (true)
	{
		//Create Named Pipe, if pipe not created, error and exit
		hMutex = CreateMutex(NULL, TRUE, myMutex);
		hPipe = CreateNamedPipe(
			lpszPipename,
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE |
			PIPE_READMODE_MESSAGE |
			PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			MAX_BUFFER_SIZE,
			MAX_BUFFER_SIZE,
			0,
			NULL);
		if (INVALID_HANDLE_VALUE == hPipe)
		{
			switch (GetLastError())
			{
			case ERROR_PIPE_BUSY:
				WaitNamedPipe(lpszPipename, 5000);
				continue;
				break;
			default:
				_tprintf(TEXT("[SERVER] CreateNamedPipe failed, Error %ld\n"),
					GetLastError());
				return -1;
				break;
			}
		}

		_tprintf(TEXT("[SERVER] Waiting for client connection...\n"));
		bConnected = ConnectNamedPipe(hPipe, NULL);
		if (TRUE == bConnected)
		{
			_tprintf(TEXT("[SERVER] Client connected, creating a processing thread.\n"));

			//Create a thread for this client.
			

			hThread = CreateThread(
				NULL,
				0,
				ThreadProc,
				(LPVOID)hPipe,
				0,
				&dwThreadId);

			//Thread created?
			if (NULL == hThread)
			{
				_tprintf(TEXT("[SERVER] CreateThread failed, Error %ld\n"),
					GetLastError());
				return -1;
			}
			else
			{
				CloseHandle(hThread);
			}//(NULL == hThread)
		}
		else
		{
			//The client not connect, so close the pipe.
			CloseHandle(hPipe);
		}//(bConnected)

	}//while

	system("PAUSE");
	return 0;
}



DWORD WINAPI ThreadProc(LPVOID lpvParam)
{
	if (NULL == lpvParam)
	{
		_tprintf(TEXT("[ThreadProc] Unexpected NULL value in lpvParam.\n"));
		_tprintf(TEXT("[ThreadProc] Exitting.\n"));
		return (DWORD)-1;
	}

	const int MAX_BUFFER_SIZE = 512;
	BOOL bSuccess = FALSE;
	DWORD dwBytesRead = 0;
	DWORD dwBytesWrite = 0;
	TCHAR szBuffer[MAX_BUFFER_SIZE] = { 0 };
	HANDLE hPipe = (HANDLE)lpvParam;
	HANDLE hPipeClient;
	char UserName[50] = "";
	char fullMesage[255] = "";
	char NamePipeClient[100];
	DWORD dwRetCode;
	

	_tprintf(TEXT("[ThreadProc] Created, receiving and processing messages.\n"));
	//пробуем читать, пока не получится
	while (bSuccess != TRUE)
	{	 
		if (bSuccess = ReadFile(
			hPipe,
			UserName,
			sizeof UserName,
			&dwBytesRead,
			NULL))
		{
			strcpy(NamePipeClient, lpszPipename);
			strcat(NamePipeClient, UserName);
			hPipeClient = CreateNamedPipe(
				NamePipeClient,
				PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_MESSAGE |
				PIPE_READMODE_MESSAGE |
				PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES,
				MAX_BUFFER_SIZE,
				MAX_BUFFER_SIZE,
				0,
				NULL);
		}
			
	}
	
	while (true)
	{

		dwRetCode = WaitForSingleObject(hMutex, INFINITE);
		// Read client requests from the pipe.
		bSuccess = ReadFile(
			hPipeClient,
			szBuffer,
			sizeof szBuffer,
			&dwBytesRead,
			NULL);

		//if request not correct
		if (
			(FALSE == bSuccess) ||
			(NULL == dwBytesRead))
		{
			if (ERROR_BROKEN_PIPE == GetLastError())
			{
				_tprintf(TEXT("[ThreadProc] Client disconnected.\n"));
			}
			else
			{
				_tprintf(TEXT("[ThreadProc] ReadFile failed, Error %ld.\n"),
					GetLastError());
			}//(ERROR_BROKEN_PIPE == GetLastError())
			break;
		}//(!bSuccess || dwBytesRead == 0)
		strcpy(fullMesage,UserName);
		strcat(fullMesage, ": ");
		strcat(fullMesage, szBuffer);
		bSuccess = WriteFile(
			hPipeClient,
			fullMesage,
			strlen(fullMesage) + 1,
			&dwBytesRead,
			NULL);

		_tprintf(TEXT("%s\n"), fullMesage);
		ReleaseMutex(hMutex);
	}//while
	FlushFileBuffers(hPipe);
	FlushFileBuffers(hPipeClient);
	DisconnectNamedPipe(hPipe);
	DisconnectNamedPipe(hPipeClient);
	CloseHandle(hPipeClient);
	CloseHandle(hPipe);
	CloseHandle(hMutex);
	_tprintf(TEXT("[ThreadProc] Exitting.\n"));
	return (DWORD)1;
}