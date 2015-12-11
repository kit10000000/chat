// 2.cpp : Defines the entry point for the application.
//9. Pipe Chat (2 ��������) 
//����������� ��� �� ���������� �������(named pipes).��� ���������� : ������ � ������.������ � GUI - ���������� �� ���������� ���������� ���������� : ���� ����� ���� ������, ���� ����� ���� �������������, ������ ��������������(��� �� �������������), ������������� ���� ����� ����������, ������ �����������, ������������� ���� �����, ��� ������������ ��� ���������.������-���������� ����������.�������� ������ �� �������, ������ ��������� �� ���� ���������.������ ���������� ������ �������������� �������� � ��������� ������.

#include "stdafx.h"
#include "2.h"
#include <cstring>
#include <windows.h> 
#pragma warning(disable : 4996)
#define MAX_LOADSTRING 100
#define ID_STR_LINE_USER 1
#define ID_BTN_CONNECT_SERVER 2
#define ID_BTN_USERNAME 3
#define ID_STR_LINE_PIPENAME 4
#define ID_RICHEDITMESSEND 5
#define ID_RICHEDITMESGET 6
#define ID_BTN_SEND 7
#define	I_MUST_READ_MY_PIPE 11101

using namespace std;
// Global Variables:
char PipeNameSystem [] = "\\\\.\\pipe\\MyPipe";
char fullMesage[255] = "";
DWORD ThreadServerId;
DWORD cbWritten;
HANDLE g_hPipeChat = 0;
HANDLE g_hPipeSystem;
HINSTANCE hInst;		
HWND hwndGetText;						// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HANDLE hMutex;
CHAR myMutex[] ="MutexName";
HANDLE g_hEvent;
char PipeName[100]; //���������� ��� �������� ������, ��������� �� ���� ��� ������
char PipeNameChat[100];
DWORD  ThreadServerIdChar[1];
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	char fullMesage[255] = "";
	BOOL bSuccess = FALSE;
	DWORD dwBytesRead = 0;
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2));
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) //������ ��������� (!)
	{
		if (msg.message == I_MUST_READ_MY_PIPE)
		{
			bSuccess = ReadFile(g_hPipeChat, fullMesage, sizeof (fullMesage), &dwBytesRead, NULL);
			if ((TRUE == bSuccess) || (NULL != dwBytesRead))
			{
				SetWindowText(hwndGetText, fullMesage);
			}
		}
			
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   DWORD MSTK;
   hInst = hInstance; // Store instance handle in our global variable
   hWnd = CreateWindow(szWindowClass, "client", WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, 460, 430, NULL, NULL, hInstance, NULL);
   LoadLibrary(TEXT("Msftedit.dll"));
   CreateWindowEx(WS_EX_STATICEDGE,"RICHEDIT50W", TEXT("Type here"),
	   ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | CBS_AUTOHSCROLL,
	   10, 80, 260, 120, hWnd, (HMENU)ID_RICHEDITMESSEND, hInstance, NULL); //���� ����� ���������
   hwndGetText = CreateWindowEx(WS_EX_STATICEDGE, "RICHEDIT50W", TEXT("Type here"),
	   ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | CBS_AUTOHSCROLL,
	   10, 210, 260, 150, hWnd, (HMENU)ID_RICHEDITMESGET, hInstance, NULL);//���� ��������� ���������
   CreateWindow("BUTTON", "Connect to server", WS_CHILD | WS_VISIBLE,
	   280, 12, 160, 25, hWnd, (HMENU)ID_BTN_CONNECT_SERVER, hInstance, NULL); //������� � �������
   CreateWindowEx(WS_EX_STATICEDGE, "EDIT", "\\\\.\\pipe\\MyPipe", WS_CHILD | WS_VISIBLE, 
	   10, 12, 260, 25, hWnd, (HMENU)ID_STR_LINE_PIPENAME, hInstance, NULL);//��� �����
   CreateWindowEx(WS_EX_STATICEDGE, "EDIT", "User Name", WS_CHILD | WS_VISIBLE, 
	   10, 45, 260, 25, hWnd, (HMENU)ID_STR_LINE_USER, hInstance, NULL);//��� �����
   CreateWindow("BUTTON", "Send", WS_CHILD | WS_VISIBLE,
	   280, 80, 80, 25, hWnd, (HMENU)ID_BTN_SEND, hInstance, NULL);//�������� ���������
   if (!hWnd)
   {
      return FALSE;
   }

   //��� ����� �� ���������, ������ ��� �� ������� �� �������, ������ �������� �� ������� �� �������� �������� � ������
   do
   {
	   hMutex = OpenMutex(SYNCHRONIZE, FALSE, myMutex);
	   g_hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, "NamedEvent");
	   MSTK = GetLastError();
   } while (hMutex == NULL);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD ThreadId;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	DWORD dwBytesRead;
	HDC hdc;
	char UserName[50] = "";
	char chatMessage[200] = "";
	BOOL connected = FALSE;
	DWORD mst;
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		COPYDATASTRUCT cd;
		BOOL work;
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_BTN_CONNECT_SERVER://����������� � ������
			if (connected)
			{
				DeleteFile(PipeNameChat);
				CloseHandle(g_hPipeChat);
				SetWindowText((HWND)lParam, "Connect");
				break;
			}
			else 
			{
				BOOL bSuccess = FALSE;
				// ����� ���� ������ GENERIC_READ | GENERIC_WRITE ����� �������� ������ ��
				g_hPipeSystem = CreateFile(PipeNameSystem, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);
				while (bSuccess != TRUE)
				{
					bSuccess = ReadFile(g_hPipeSystem, (void*)ThreadServerIdChar, sizeof(ThreadServerIdChar), &dwBytesRead, NULL);//��� ������ ������������ � �������, ��������� ���� ��� ������, ����� �� ����� ��� ��������� ���������, ��� �� ��������� ��������� � ���
					mst = GetLastError();
				}
//				ThreadServerId = (DWORD)ThreadServerIdChar;
				DWORD CurrentThreadId = GetCurrentThreadId();
				 PostThreadMessage((DWORD)ThreadServerIdChar[0], CurrentThreadId, 0, (LPARAM)&cd);//��������� ��� ������� id ������ �������, ����� �� ��� �������� ��� � ������ ���� ���� ������� ���� ��������
				mst = GetLastError();
				if (g_hPipeSystem != INVALID_HANDLE_VALUE) // �������, ���� ����� ������, �� ���������� ���������� �� ������
				{
					SetWindowText((HWND)lParam, "Disconnect");
					connected = TRUE;
				}
				GetDlgItemText(hWnd, ID_STR_LINE_USER, UserName, 255);
				WriteFile(g_hPipeSystem, UserName, strlen(UserName) + 1, &cbWritten, NULL);//��������� � ��������� ���� Username, ������ ��� �� ���� ����� ������������������ ����� ����������
				strcpy(PipeNameChat, PipeNameSystem);
				strcat(PipeNameChat, UserName);
				//Sleep(100);//����� ��� �������� ��������� ���(�� �������)
				//����� ����� ������������� ��-�� GENERIC_READ | GENERIC_WRITE
				// �� ���� ���-�� ����, �� �������� ������-��				
				mst = GetLastError();
				break;
			}
		case ID_BTN_SEND:
			if (!g_hPipeChat)
				g_hPipeChat = CreateFile(PipeNameChat, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL); //���������(!)
			WaitForSingleObject(hMutex, INFINITE);
			GetDlgItemText(hWnd, ID_RICHEDITMESSEND, chatMessage, 255);
			GetDlgItemText(hWnd, ID_STR_LINE_USER, UserName, 255);
			WriteFile(g_hPipeChat, chatMessage, strlen(chatMessage) + 1, &cbWritten, NULL);
			SetEvent(g_hEvent);
			ReleaseMutex(hMutex);
			ThreadId = GetCurrentThreadId();
			work = PostThreadMessage(ThreadServerIdChar[0], (UINT)ThreadId, 0, (LPARAM)&cd); //!!!!!!!!!!!!!!!!!!!!
//			strcpy(fullMesage, UserName);
//			strcat(fullMesage, ": ");
//			strcat(fullMesage,  chatMessage);
//			strcat(fullMesage, "\n");
//			SetWindowText(hwndGetText, fullMesage);
			Sleep(200);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		DeleteFile(PipeNameChat);
		DeleteFile(PipeNameSystem);
		CloseHandle(g_hPipeSystem);
		CloseHandle(g_hPipeChat);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

