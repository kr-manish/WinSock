#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

std::string local_server = "localhost";

//This will tell the linker that this library is needed
#pragma comment(lib, "WS2_32.lib")

int client(void){
	WSADATA wsaData;
	int iResult = 0;
	SOCKET connectsocket = INVALID_SOCKET;
	int recvbuflen = DEFAULT_BUFLEN;
	char *sendbuf = "This is a test!!";
	char recvbuf[DEFAULT_BUFLEN] = { 0 };

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0){
		DWORD LastError = GetLastError();
		std::cout << "Error " << LastError << ":Not able to start WSAStatup!!\n";
		return 1;
	}

	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//resolve the server address and port
	iResult = getaddrinfo(local_server.c_str(), DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	ptr = result;
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next){
		//creating the socket
		connectsocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (connectsocket == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		//connecting to the socket
		iResult = connect(connectsocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(connectsocket);
			connectsocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);
	if (connectsocket == INVALID_SOCKET){
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	//sending the data
	iResult = send(connectsocket, sendbuf, strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(connectsocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes sent: %d\n", iResult);

	//shutdown the connection for sending since no more data to be sent
	//still can use connectsocket to receive data
	iResult = shutdown(connectsocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(connectsocket);
		WSACleanup();
		return 1;
	}

	do
	{
		iResult = recv(connectsocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received!!: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection Closed\n");
		else
			printf("recv failed! %d\n", WSAGetLastError());
	} while (iResult > 0);

	closesocket(connectsocket);
	WSACleanup();

	return 0;
}