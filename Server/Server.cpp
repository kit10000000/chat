#include "stdafx.h"
#include <vector>
#include <windows.h>
#include <stdio.h> 
#include <tchar.h>
#include <list>
#define	I_MUST_READ_MY_PIPE 11101
#define MAX_BUFFER_SIZE 512
#pragma warning(disable : 4996)
using namespace std;
DWORD WINAPI ThreadProc(LPVOID);
vector<DWORD> ThreadsId;//список всех id потоков - клиентов
vector<LPSTR> ListOfUserNames;
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\MyPipe");
HANDLE hMutex;
CHAR myMutex[] ="MutexName";
HANDLE hEvent;
HANDLE hPipeClient;
char fullMesage[255] = "";
HANDLE hEventRd;
OVERLAPPED OVLRd;
HANDLE Clients[];
int ClientsCount = 0;
int ClientInd;
DWORD BTR;
HANDLE hPipe = INVALID_HANDLE_VALUE;
HANDLE hThread = NULL;
BOOL bConnected = FALSE;
DWORD dwThreadId = NULL;
MSG msg;
DWORD dwBytesRead;
BOOL bSuccess;
COPYDATASTRUCT cd;
int _tmain(VOID)
{
	BOOL flagPeekMsg = FALSE;
	while (true)
	{
		hMutex = CreateMutex(NULL, FALSE, myMutex); //
		hEvent = CreateEvent(NULL, FALSE, FALSE, "NamedEvent"); //
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
		hEventRd = CreateEvent(NULL, TRUE, FALSE, NULL);
		OVLRd.hEvent = hEventRd;
		bConnected = ConnectNamedPipe(hPipe, NULL);
		if (bConnected == TRUE)
		{
			DWORD ThreadMainId = GetCurrentThreadId();
			_tprintf(TEXT("bla2\n"));
			bSuccess = WriteFile(hPipe, (LPCVOID)&ThreadMainId, sizeof(LPCVOID)+1, &dwBytesRead, NULL);
			DWORD MST = GetLastError();
			MSG Msg;		
			if (!flagPeekMsg)
			{ 
				flagPeekMsg = PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
				flagPeekMsg = TRUE;
				ThreadsId.push_back(Msg.message);
			}
			MST = GetLastError();
			_tprintf(TEXT("[SERVER] Client connected, creating a processing thread.\n"));
			hThread = CreateThread(NULL,0,ThreadProc,(LPVOID)hPipe,0,&dwThreadId); //
			if (NULL == hThread)
			{
				_tprintf(TEXT("[SERVER] CreateThread failed, Error %ld\n"),
					GetLastError());
				return -1;
			}
			else
			{
				CloseHandle(hThread);
			}//(NULL == hThread)(старое)
		}
		else
		{
			//The client not connect, so close the pipe.(старое)
			CloseHandle(hPipe);
		}//(bConnected)(старое)
	}//while(старое)
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
	BOOL bSuccess = FALSE;
	DWORD dwBytesRead = 0;
	TCHAR szBuffer[MAX_BUFFER_SIZE] = { 0 };
	HANDLE hPipe = (HANDLE)lpvParam;
	char UserName[50] = "";
	char NamePipeClient[100];
	DWORD mist;
	_tprintf(TEXT("[ThreadProc] Created, receiving and processing messages.\n"));
	while (bSuccess != TRUE)
	{	
		if (bSuccess = ReadFile(hPipe,UserName,sizeof(UserName),&dwBytesRead,&OVLRd))
		{ 
			LPSTR NameOfUser = UserName;
			ListOfUserNames.push_back(NameOfUser);
			_tprintf(TEXT("bla\n"));
			strcpy(NamePipeClient, lpszPipename);
			strcat(NamePipeClient, UserName);
			hPipeClient = CreateNamedPipe(NamePipeClient,PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE |PIPE_READMODE_MESSAGE |PIPE_NOWAIT,PIPE_UNLIMITED_INSTANCES,MAX_BUFFER_SIZE,MAX_BUFFER_SIZE,0,NULL);
			mist = GetLastError();
		}	
	}
	while (true)
	{
		WaitForSingleObject(hEvent, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);
		bSuccess = ReadFile(hPipeClient,szBuffer,sizeof(szBuffer),&dwBytesRead,&OVLRd);
		strcpy(fullMesage, UserName);
		strcat(fullMesage, ": ");
		strcat(fullMesage, szBuffer);
		_tprintf(TEXT("bla1\n"));
		if ((FALSE == bSuccess) ||(NULL == dwBytesRead))
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
			break;
		}
		ReleaseMutex(hMutex);
		GetOverlappedResult(hPipeClient, &OVLRd, &BTR, FALSE);
		vector <int>::size_type size = ListOfUserNames.size();
		for (unsigned int i = 0; i < size; i++)
		{
			_tprintf(TEXT("bla3\n"));
			LPSTR NamePipeClient = new CHAR[255];
			NamePipeClient = lpszPipename;
			lstrcat(NamePipeClient, ListOfUserNames[i]);
			HANDLE hPipeSystem = CreateFile((LPCSTR)NamePipeClient, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);
			bSuccess = WriteFile(hPipeSystem, (LPCVOID)fullMesage, sizeof(DWORD)+1, &dwBytesRead, NULL);
			DeleteFile(NamePipeClient);
			CloseHandle(hPipeSystem);
		}
		DWORD mistakefind;
		DWORD iwantittobemythredId;
		vector<DWORD>::iterator it;
		bool work;
		for (it = ThreadsId.begin(); it != ThreadsId.end(); it++)
		{
			iwantittobemythredId = (DWORD) *it;
			work = PostThreadMessage(iwantittobemythredId, I_MUST_READ_MY_PIPE, 0, (LPARAM)&cd);
			mistakefind = GetLastError();
		}
//		}
//		}
	_tprintf(TEXT("%s\n"), fullMesage);
	}//while(старое)
	FlushFileBuffers(hPipe);
	FlushFileBuffers(hPipeClient);
	DisconnectNamedPipe(hPipe);
	DisconnectNamedPipe(hPipeClient);
	CloseHandle(hPipeClient);
	CloseHandle(hPipe);
	CloseHandle(hMutex);
	_tprintf(TEXT("[ThreadProc] Exitting.1\n"));
	return (DWORD) 1;
} //каждый киент к серверу создаёт по два пайпа 