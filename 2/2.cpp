// 2.cpp : Defines the entry point for the application.
//9. Pipe Chat (2 человека) 
//Реализовать чат на именованых каналах(named pipes).Два приложения : клиент и сервер.Клиент — GUI - приложение со следующими элементами управления : поле ввода «Имя канала», поле ввода «Имя пользователя», кнопка «Подключиться»(она же «Отключиться»), многострочное поле ввода «Сообщение», кнопка «Отправить», многострочное поле ввода, где отображаются все сообщения.Сервер-консольное приложение.Принимая данные от клиента, сервер рассылает их всем остальным.Каждое соединение должно обрабатываться сервером в отдельном потоке.

#include "stdafx.h"
#include "2.h"
#include <cstring>
#include <windows.h> 
#include <RichEdit.h>
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
#include <forward_list>

using namespace std;
// Global Variables:
char PipeNameSystem [] = "\\\\.\\pipe\\MyPipe";

DWORD ThreadServerId;
HANDLE hEvent;
DWORD cbWritten;
char NamePipeClientIn[100];
char NamePipeClientOut[100];
HANDLE g_hPipeChat = INVALID_HANDLE_VALUE;
HANDLE g_hPipeSystem;
HINSTANCE hInst;
DWORD ThreadId;
HWND hwndGetText;						// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HANDLE g_hEvent;
char PipeName[100]; //переменная для создания канала, считывает из поля имя канала
char PipeNameChat[100];
DWORD  ThreadServerIdChar[1];
HANDLE ThreadForReading;
HWND hWnd;
HANDLE ReadingPipe = INVALID_HANDLE_VALUE;
char UserName[50] = "";
DWORD WINAPI ReadFunc(LPVOID lParam);
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
	while (GetMessage(&msg, NULL, 0, 0)) //клиент считывает 
	{		
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
   hInst = hInstance; // Store instance handle in our global variable
   hWnd = CreateWindow(szWindowClass, "client", WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, 460, 430, NULL, NULL, hInstance, NULL);
   LoadLibrary(TEXT("Msftedit.dll"));
   CreateWindowEx(WS_EX_STATICEDGE,"RICHEDIT50W", TEXT("Type here"),
	   ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | CBS_AUTOHSCROLL,
	   10, 80, 260, 120, hWnd, (HMENU)ID_RICHEDITMESSEND, hInstance, NULL); //поле ввода сообщений
   hwndGetText = CreateWindowEx(WS_EX_STATICEDGE, "RICHEDIT50W", nullptr,
	   ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | CBS_AUTOHSCROLL,
	   10, 210, 260, 150, hWnd, (HMENU)ID_RICHEDITMESGET, hInstance, NULL);//поле получения сообщений
   CreateWindow("BUTTON", "Connect to server", WS_CHILD | WS_VISIBLE,
	   280, 12, 160, 25, hWnd, (HMENU)ID_BTN_CONNECT_SERVER, hInstance, NULL); //коннект к серверу
   CreateWindowEx(WS_EX_STATICEDGE, "EDIT", "\\\\.\\pipe\\MyPipe", WS_CHILD | WS_VISIBLE, 
	   10, 12, 260, 25, hWnd, (HMENU)ID_STR_LINE_PIPENAME, hInstance, NULL);//имя пайпа
   CreateWindowEx(WS_EX_STATICEDGE, "EDIT", "User Name", WS_CHILD | WS_VISIBLE, 
	   10, 45, 260, 25, hWnd, (HMENU)ID_STR_LINE_USER, hInstance, NULL);//имя юзера
   CreateWindow("BUTTON", "Send", WS_CHILD | WS_VISIBLE,
	   280, 80, 80, 25, hWnd, (HMENU)ID_BTN_SEND, hInstance, NULL);//отправка сообщения
   ThreadForReading = CreateThread(NULL, 0, ReadFunc, 0, 0, &ThreadId);
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
DWORD WINAPI ReadFunc(LPVOID lParam)
{
	char BufferForClientMessage[256];
	DWORD dwBytesRead;
	bool bSuccess;
	CHARRANGE cr;
	cr.cpMin = -1;
	cr.cpMax = -1;
	while (true)
	{
		bSuccess = FALSE;
		//чтение из канала в клиенте
		if (ReadingPipe != INVALID_HANDLE_VALUE)
		{
			bSuccess = ReadFile(ReadingPipe, BufferForClientMessage, sizeof(BufferForClientMessage), &dwBytesRead, NULL);
		}
		
		if (bSuccess)
		{
			SendMessage(hwndGetText, EM_EXSETSEL, 0, (LPARAM)&cr);
			SendMessage(hwndGetText, EM_REPLACESEL, 0, (LPARAM)BufferForClientMessage);
			SendMessage(hwndGetText, EM_REPLACESEL, 0, (LPARAM)"\n");
		}
	}
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	char chatMessage[200] = "";
	BOOL connected = FALSE;
	char fullMesage[255] = "";
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
		case ID_BTN_CONNECT_SERVER://подключение к каналу(старое)
			if (connected)
			{
				CloseHandle(g_hPipeChat);
				SetWindowText((HWND)lParam, "Connect");
				break;
			}
			else 
			{
				BOOL bSuccess = FALSE;
				g_hPipeSystem = CreateFile(PipeNameSystem, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);
				GetDlgItemText(hWnd, ID_STR_LINE_USER, UserName, 255);
				WriteFile(g_hPipeSystem, UserName, strlen(UserName) + 1, &cbWritten,NULL);
				strcpy(NamePipeClientIn, PipeNameSystem);
				strcat(NamePipeClientIn, UserName);	
				strcat(NamePipeClientIn, "_in");
				strcpy(NamePipeClientOut, PipeNameSystem);
				strcat(NamePipeClientOut, UserName);
				strcat(NamePipeClientOut, "_out");
				
			}
			break;
		case ID_BTN_SEND:
//			DebugBreak();
			if (g_hPipeChat == INVALID_HANDLE_VALUE)
				g_hPipeChat = CreateFile(NamePipeClientIn, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL); //проверить(!)(старое)
			if (ReadingPipe == INVALID_HANDLE_VALUE)
				ReadingPipe = CreateFile(NamePipeClientOut, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);
			g_hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, "NamedEvent");
			GetDlgItemText(hWnd, ID_RICHEDITMESSEND, chatMessage, 255);
			strcpy(fullMesage, UserName);
			strcat(fullMesage, ": ");
			strcat(fullMesage, chatMessage);
			WriteFile(g_hPipeChat, fullMesage, strlen(fullMesage) + 1, &cbWritten, NULL);
			break;
		case IDM_EXIT:
			CloseHandle(g_hPipeChat);
			CloseHandle(g_hPipeSystem);
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...(старое)
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		CloseHandle(g_hPipeChat);
		CloseHandle(g_hPipeSystem);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

