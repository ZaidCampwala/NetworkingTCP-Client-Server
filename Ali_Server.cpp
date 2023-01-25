#undef UNICODE

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <process.h>

#pragma comment (lib,"Ws2_32.lib")

#define DEFAULT_PORT "27000"

#define DEFAULT_BUFLEN 512

unsigned __stdcall ClientSession(void* data)
{
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	int iResult;
	int iSendResult;

	SOCKET ClientSocket = (SOCKET)data;

	do
	{
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);

			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}

			printf("Received Message %.*s\n", iResult, recvbuf);
		}
		else if (iResult == 0)
		{
			printf("Connection closing... \n");
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}


	} while (iResult > 0);

	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	closesocket(ClientSocket);
	return 1;
}


int main()
{

	WSADATA wsaData;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}


	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}


	SOCKET ListenSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, int(result->ai_addrlen));
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with errir: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Listen failed with errir: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	while (ClientSocket = accept(ListenSocket, NULL, NULL))
	{
		if (ClientSocket == INVALID_SOCKET)
		{
			printf("accept failed with errir: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		unsigned threadID;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ClientSession, (void*)ClientSocket, 0, &threadID);

	}

	WSACleanup();
	printf("Shutting down");
	return 0;
}
#endif