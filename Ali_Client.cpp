#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27000"
#define DEFAULT_BUFLEN 512

int main(int argc, char** argv) {
	WSADATA wsaData;
	int iResult;

	SOCKET ConnectSocket = INVALID_SOCKET;


	char sendbuf[] = "test data";
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int totalBytesRecv = 0;



	struct addrinfo* result = NULL, * ptr = NULL, hints;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		printf("WSAStartup Failed!: %d\n", iResult);
		system("pause");
		return 1;
	}//endif

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;

	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddinfo failed: %d\n: ", iResult);
		WSACleanup();
		system("pause");
		return 1;
	}//end if

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("Socket failed with error: %d\n: ", WSAGetLastError());
			WSACleanup();
			system("pause");
			return 1;
		}//end if
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}//end if
		break;
	}//end for 


	do {

	std:: cout << "Please enter the message (type !exit to disconnect)" << std::endl;
	std::cin >> sendbuf;
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);

		if (iResult > 0) {
			if (strcmp(sendbuf, "!exit") == 0)
			{
				printf("User typed !exit - Disconnecting from Server\n");
				printf("Bytes Received : %d\n", totalBytesRecv);
				break;
			}
			else {
				printf("Message Length: %d\n", strlen(sendbuf));
				totalBytesRecv += iResult;
			}
		}	else {

			printf("recv failed with error: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		system("pause");
		return 1;
	}//end if

	closesocket(ConnectSocket);
	WSACleanup();

	printf("CLIENT END\n");
	system("pause");

	return 0;
}//end main