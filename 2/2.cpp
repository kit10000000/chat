// 2.cpp : Defines the entry point for the application.
//9. Pipe Chat (2 человека) 
//Реализовать чат на именованых каналах(named pipes).Два приложения : клиент и сервер.Клиент — GUI - приложение со следующими элементами управления : поле ввода «Имя канала», поле ввода «Имя пользователя», кнопка «Подключиться»(она же «Отключиться»), многострочное поле ввода «Сообщение», кнопка «Отправить», многострочное поле ввода, где отображаются все сообщения.Сервер-консольное приложение.Принимая данные от клиента, сервер рассылает их всем остальным.Каждое соединение должно обрабатываться сервером в отдельном потоке.

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

using namespace std;
// Global Variables:
char PipeNameSystem [] = "\\\\.\\pipe\\MyPipe";
DWORD cbWritten;
HANDLE g_hPipeChat;
HANDLE g_hPipeSystem;
HINSTANCE hInst;		
HWND hwndGetText;						// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HANDLE hMutex;
CHAR myMutex[] ="MutexName";
HANDLE g_hEvent;
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
	while (GetMessage(&msg, NULL, 0, 0)) //клиент считывает (!)
	{
		bSuccess = ReadFile(g_hPipeChat,fullMesage,sizeof (fullMesage),&dwBytesRead,NULL);
		if ((TRUE == bSuccess) || (NULL != dwBytesRead))
		{
			SetWindowText(hwndGetText, fullMesage);
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
	   10, 80, 260, 120, hWnd, (HMENU)ID_RICHEDITMESSEND, hInstance, NULL); //поле ввода сообщений
   hwndGetText = CreateWindowEx(WS_EX_STATICEDGE, "RICHEDIT50W", TEXT("Type here"),
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
   if (!hWnd)
   {
      return FALSE;
   }

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
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	char PipeName[100]; //переменная для создания канала, считывает из поля имя канала
	char PipeNameChat[100];
	char UserName[50] = "";
	char chatMessage[200] = "";
	char fullMesage[255] = "";
	BOOL connected = FALSE;
	DWORD mst;
	switch (message)
	{
		
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_BTN_CONNECT_SERVER://подключение к каналу
			if (connected)
			{
				DeleteFile(PipeName);
				SetWindowText((HWND)lParam, "Connect");
				break;
			}
			else 
			{
				GetDlgItemText(hWnd, ID_STR_LINE_PIPENAME, PipeName, 255);
				// здесь если ставлю GENERIC_READ | GENERIC_WRITE прога начинает тупить хз
				g_hPipeSystem = CreateFile(PipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//канал для записи
				if (g_hPipeSystem != INVALID_HANDLE_VALUE) // условие, если канал создан, то отображает дисконнект на кнопке
				{
					SetWindowText((HWND)lParam, "Disconect");
					connected = TRUE;
				}
				GetDlgItemText(hWnd, ID_STR_LINE_USER, UserName, 255);
				WriteFile(g_hPipeSystem, UserName, strlen(UserName) + 1, &cbWritten, NULL);
				strcpy(PipeNameChat, PipeName);
				strcat(PipeNameChat, UserName);
				//Sleep(100);//чтобы там успелось создаться все(на сервере)
				//ЗДЕСЬ МОЖЕТ ЗАСТОПОРИТЬСЯ ИЗ-ЗА GENERIC_READ | GENERIC_WRITE
				// НО ЕСЛИ ЧТО-ТО ОДНО, ТО РАБОТАЕТ ПОЧЕМУ-ТО
				g_hPipeChat = CreateFile(PipeNameChat, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); //проверить(!)
				break;
			}
		case ID_BTN_SEND:
			WaitForSingleObject(hMutex, INFINITE);
			GetDlgItemText(hWnd, ID_RICHEDITMESSEND, chatMessage, 255);
			WriteFile(g_hPipeChat, chatMessage, strlen(chatMessage) + 1, &cbWritten, NULL);
			SetEvent(g_hEvent);
			ReleaseMutex(hMutex);
			mst = GetLastError();
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
		DeleteFile(PipeName);
		DeleteFile(PipeNameSystem);
		CloseHandle(g_hPipeSystem);
		CloseHandle(g_hPipeChat);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

