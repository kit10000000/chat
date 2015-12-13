#include "stdafx.h"
#include <vector>
#include <windows.h>
#include <stdio.h> 
#include <tchar.h>
#include <string>
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
//################################
HANDLE Clients[];
int ClientsCount = 0;
int ClientInd;
DWORD BTR;
//##########################
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
		//вот здесь встает и дальше не идет
		//возможно это из-за неверного указания HWND
		
		//Create Named Pipe, if pipe not created, error and exit
		hMutex = CreateMutex(NULL, FALSE, myMutex);
		hEvent = CreateEvent(NULL, FALSE, FALSE, "NamedEvent");
		//|PIPE_WAIT стояло во втором параметре, я убрал проверить
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
//######################################################################
			ClientsCount++;
			ClientInd = ClientsCount;
//#############################
			DWORD ThreadMainId = GetCurrentThreadId();
			//посылаем id потока этого, чтобы потом могли обмениваться сообщениями с клиентом, а то клиент не будет знать, куда отправлять сообщение(старое)
			_tprintf(TEXT("bla2\n"));
			bSuccess = WriteFile(hPipe, (LPCVOID)&ThreadMainId, sizeof(LPCVOID)+1, &dwBytesRead, NULL);
			DWORD MST = GetLastError();
//			DWORD ThreadClientId;(старое)
			MSG Msg;
			
			if (!flagPeekMsg)
//			do //пока клиент не отправит нам айди своего потока, чтобы мы могли добавить его в список всех айди потоков(старое)
			{ 
				flagPeekMsg = PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
				flagPeekMsg = TRUE;
				ThreadsId.push_back(Msg.message);
			}// while (flagPeekMsg != TRUE);
			MST = GetLastError();
			_tprintf(TEXT("[SERVER] Client connected, creating a processing thread.\n"));
			//Create a thread for this client.(старое)
			hThread = CreateThread(NULL,0,ThreadProc,(LPVOID)hPipe,0,&dwThreadId);//(!!!!!)может быть трабл? call stack предупреждает(старое)	
			//Thread created?(старое)
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
	
	//пробуем читать, пока не получится(старое)
	//создаем здесь пайп для клиента(старое)
	while (bSuccess != TRUE)
	{	
		if (bSuccess = ReadFile(hPipe,UserName,sizeof(UserName),&dwBytesRead,&OVLRd))
		{ 
			//(!!!)
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
		//if request not correct(старое)
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
			break;//(ERROR_BROKEN_PIPE == GetLastError())(старое)
		}//(!bSuccess || dwBytesRead == 0)(старое)
		ReleaseMutex(hMutex);
//		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//		{	
		GetOverlappedResult(hPipeClient, &OVLRd, &BTR, FALSE);
//			for (it = ThreadsId.begin(); it != ThreadsId.end(); it++) //проверка того, что у нас действительно сообщение от потока пришли, а не от чего-то другого
//			if ((DWORD)msg.message == *it){
				//сначала пишем во все пайпы все сообщения 
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
				for (it = ThreadsId.begin(); it != ThreadsId.end(); it++)
				{
					iwantittobemythredId = (DWORD) *it;
					work = PostThreadMessage(iwantittobemythredId, I_MUST_READ_MY_PIPE, 0, (LPARAM)&cd);
					mistakefind = GetLastError();
				}
//			}
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
}