#include "stdafx.h"
#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>

DWORD WINAPI ThreadProc(LPVOID);

int _tmain(VOID)
{
	const int MAX_BUFFER_SIZE = 512;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\MyPipe");
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	HANDLE hThread = NULL;
	BOOL bConnected = FALSE;
	DWORD dwThreadId = NULL;
	while (true)
	{
		//Create Named Pipe, if pipe not created, error and exit
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
	_tprintf(TEXT("[ThreadProc] Created, receiving and processing messages.\n"));

	while (true)
	{
		// Read client requests from the pipe.
		bSuccess = ReadFile(
			hPipe,
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

		bSuccess = WriteFile(
			hPipe,
			szBuffer,
			_tcslen(szBuffer + 1) * sizeof(TCHAR),
			&dwBytesRead,
			NULL);

		_tprintf(TEXT("%s\n"), szBuffer);
	}//while
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	_tprintf(TEXT("[ThreadProc] Exitting.\n"));
	return (DWORD)1;
}