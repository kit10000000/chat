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
int _tmain(VOID)
{
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
		if (bConnected == TRUE)
		{
			BOOL bSuccess = FALSE;
			DWORD dwBytesRead = 0;
			TCHAR szBuffer[MAX_BUFFER_SIZE] = { 0 };
			char UserName[50] = "";
			char NamePipeClient[100];
			DWORD mist;
			_tprintf(TEXT("[ThreadProc] Created, receiving and processing messages.\n"));
			while (bSuccess != TRUE)
			{
				if (ReadFile(hPipe, UserName, sizeof(UserName), &dwBytesRead, NULL))
				{
					LPSTR NameOfUser = UserName;
					ListOfUserNames.push_back(NameOfUser);
					strcpy(NamePipeClient, lpszPipename);
					strcat(NamePipeClient, UserName);
					hPipeClient = CreateNamedPipe(NamePipeClient, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, PIPE_UNLIMITED_INSTANCES, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, 0, NULL);
					mist = GetLastError();
				}
				WaitForSingleObject(hEvent, INFINITE);
				bSuccess = ReadFile(hPipeClient, szBuffer, sizeof(szBuffer), &dwBytesRead, NULL);//чтение сообщения из клиента
				strcpy(fullMesage, UserName);
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
					break;
				}
				vector <int>::size_type size = ListOfUserNames.size();
				DWORD mistake;
				for (unsigned int i = 0; i < size; i++)
				{
					_tprintf(TEXT("bla3\n"));
					LPTSTR NamePipeClient = new CHAR [200];
					lstrcpy(NamePipeClient, lpszPipename);
					lstrcat(NamePipeClient, ListOfUserNames[i]);
					HANDLE PipeForSending = CreateFile((LPCSTR)NamePipeClient, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);
					mistake = GetLastError();
					bSuccess = WriteFile(PipeForSending, (LPCVOID)fullMesage, sizeof(DWORD)+1, &dwBytesRead, NULL);
					DeleteFile(NamePipeClient);
					CloseHandle(PipeForSending);
				}
				_tprintf(TEXT("%s\n"), fullMesage);
			}//while(старое)
		}
		else
		{
			//The client not connect, so close the pipe.(старое)
			FlushFileBuffers(hPipe);
			FlushFileBuffers(hPipeClient);
			DisconnectNamedPipe(hPipe);
			DisconnectNamedPipe(hPipeClient);
			CloseHandle(hPipeClient);
			CloseHandle(hPipe);
			_tprintf(TEXT("[ThreadProc] Exitting.1\n"));
			return (DWORD)1;
		}//(bConnected)(старое)
	}//while(старое)

}