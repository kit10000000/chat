// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <thread>
#include "windows.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hPipe = CreateNamedPipe("\\\\.\\pipe\\MyPipe", PIPE_ACCESS_INBOUND, PIPE_TYPE_MESSAGE, PIPE_UNLIMITED_INSTANCES, 255, 255, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	ConnectNamedPipe(hPipe, NULL);
	std::string clientPipeName[512];
	DWORD cbRead;
	
	while (true)
	{
		if (ReadFile(hPipe, clientPipeName, 512, &cbRead, NULL))
		{
			auto fullName = "\\\\.\\pipe\\MyPipe" + clientPipeName;
		}
	}
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	return 0;
}

