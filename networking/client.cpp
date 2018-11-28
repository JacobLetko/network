#include "loki.h"
#include <stdio.h>
#include <WinSock2.h>

int main()
{
	WORD winsock_verision = 0x202;
	WSADATA winsock_data;
	if (WSAStartup(winsock_verision, &winsock_data))
	{
		printf("WSAStartup failed: %d", WSAGetLastError());
		return;
	}

	int address_family = AF_INET;
	int type = SOCK_DGRAM;
	int protocol = IPPROTO_UDP;
	SOCKET sock = socket(address_family, type, protocol);

	if (sock == INVALID_SOCKET)
	{
		printf("Socket Faild: %d", WSAGetLastError());
		return;
	}

	SOCKADDR_IN server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	char message[SOCKET_BUFFER_SIZE];
	gets_s(message, SOCKET_BUFFER_SIZE);
	int flags = 0;
	if (sendto(sock, message, strlen(message), flags, (SOCKADDR*)&server_address, sizeof(server_address)) == SOCKET_ERROR)
	{
		printf("sendto faield: %d", WSAGetLastError());
		return;
	}

	printf("done");

	return 0;
}
