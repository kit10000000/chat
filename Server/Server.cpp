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
vector<DWORD> ThreadsId;//������ ���� id ������� - ��������
vector<LPSTR> ListOfUserNames;
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\MyPipe");
HANDLE hMutex;
CHAR myMutex[] ="MutexName";
HANDLE hEvent;
HANDLE hPipeClient;
char fullMesage[255] = "";


int _tmain(VOID)
{
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	HANDLE hThread = NULL;
	BOOL bConnected = FALSE;
	DWORD dwThreadId = NULL;
	MSG msg;
	DWORD dwBytesRead;
	BOOL bSuccess;
	vector<DWORD>::iterator it;
	COPYDATASTRUCT cd;
	while (true)
	{
		//��� ����� ������ � ������ �� ����
		//�������� ��� ��-�� ��������� �������� HWND
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			for (it = ThreadsId.begin(); it != ThreadsId.end(); it++) //�������� ����, ��� � ��� ������������� ��������� �� ������ ������, � �� �� ����-�� �������
				if ((DWORD)msg.message == *it){
				//������� ����� �� ��� ����� ��� ��������� 
				vector <int>::size_type size = ListOfUserNames.size();
				for (int i = 0; i < size; i++)
				{
					_tprintf(TEXT("bla3\n"));
					//(!!!)
					LPSTR NamePipeClient = new CHAR[255];
					NamePipeClient = lpszPipename;
					lstrcat(NamePipeClient, ListOfUserNames[i]);
					HANDLE hPipeSystem = CreateFile((LPCSTR)NamePipeClient, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);
					bSuccess = WriteFile(hPipeSystem, (LPCVOID)fullMesage, sizeof(DWORD)+1, &dwBytesRead, NULL);
					DeleteFile(NamePipeClient);
					CloseHandle(hPipeSystem);
				}
				//����� �������� ���������
				for (it = ThreadsId.begin(); it != ThreadsId.end(); it++)//�������� ���� �������� ��������� � ���, ��� ���� ������ �����
				{
					PostThreadMessage((DWORD)&it, I_MUST_READ_MY_PIPE, 0, (LPARAM)&cd);
				}
			}
		}
		//Create Named Pipe, if pipe not created, error and exit
		hMutex = CreateMutex(NULL, FALSE, myMutex);
		hEvent = CreateEvent(NULL, FALSE, FALSE, "NamedEvent");
		//|PIPE_WAIT ������ �� ������ ���������, � ����� ���������
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
		if (bConnected == TRUE)
		{
			DWORD ThreadMainId = GetCurrentThreadId();
			//�������� id ������ �����, ����� ����� ����� ������������ ����������� � ��������, � �� ������ �� ����� �����, ���� ���������� ���������
			_tprintf(TEXT("bla2\n"));
			bSuccess = WriteFile(hPipe, (LPCVOID)&ThreadMainId, sizeof(LPCVOID)+1, &dwBytesRead, NULL);
			DWORD MST = GetLastError();
			DWORD ThreadClientId;
			MSG Msg;
			BOOL flagPeekMsg;
			do //���� ������ �� �������� ��� ���� ������ ������, ����� �� ����� �������� ��� � ������ ���� ���� �������
			{ 
				flagPeekMsg = PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
			} while (flagPeekMsg != TRUE);
			ThreadsId.push_back(Msg.message);
			MST = GetLastError();
			_tprintf(TEXT("[SERVER] Client connected, creating a processing thread.\n"));
			//Create a thread for this client.
			hThread = CreateThread(NULL,0,ThreadProc,(LPVOID)hPipe,0,&dwThreadId);//(!!!!!)����� ���� �����? call stack �������������	
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
	BOOL bSuccess = FALSE;
	DWORD dwBytesRead = 0;
	TCHAR szBuffer[MAX_BUFFER_SIZE] = { 0 };
	HANDLE hPipe = (HANDLE)lpvParam;
	char UserName[50] = "";
	char NamePipeClient[100];
	DWORD mist;
	_tprintf(TEXT("[ThreadProc] Created, receiving and processing messages.\n"));
	
	//������� ������, ���� �� ���������
	//������� ����� ���� ��� �������
	while (bSuccess != TRUE)
	{	
		if (bSuccess = ReadFile(hPipe,UserName,sizeof(UserName),&dwBytesRead,NULL))
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
		// Read client requests from the pipe.
		//����� �������� ��� ������ ������, �������, ��� ������ ����
		bSuccess = ReadFile(hPipeClient,szBuffer,sizeof(szBuffer),&dwBytesRead,NULL);
		//if request not correct
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
			break;//(ERROR_BROKEN_PIPE == GetLastError())
		}//(!bSuccess || dwBytesRead == 0)
		strcpy(fullMesage,UserName);
		strcat(fullMesage, ": ");
		strcat(fullMesage, szBuffer);
		//bSuccess = WriteFile(hPipeClient,fullMesage,strlen(fullMesage) + 1,&dwBytesRead,NULL);
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
	_tprintf(TEXT("[ThreadProc] Exitting.1\n"));
	return (DWORD) 1;
}