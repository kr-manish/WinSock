// WinSock.cpp : Defines the entry point for the console application.

//winsock2.h internally includes windows.h. If one declares it, should be preceded with following to avoid conflicts.
#include "stdafx.h"
#include <Windows.h>
#include "server.h"

int _tmain(int argc, _TCHAR* argv[])
{
	server();
	return 0;
}

