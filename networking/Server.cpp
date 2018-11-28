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

	SOCKADDR_IN local_address;
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons(2022);
	local_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
	{
		printf("bind Failed: %d", WSAGetLastError());
	}

	char buffer[SOCKET_BUFFER_SIZE];
	int flags = 0;
	SOCKADDR_IN from;
	int from_size = sizeof(from);
	int bytes_recived = recvfrom(sock, buffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);
	if (bytes_recived == SOCKET_ERROR)
	{
		printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d", WSAGetLastError());
	}
	else
	{
		buffer[bytes_recived] = 0;
		printf("%d.%d.%d.%d:%d - %s", from.sin_addr.S_un.S_un_b.s_b1, from.sin_addr.S_un.S_un_b.s_b2, from.sin_addr.S_un.S_un_b.s_b3, from.sin_addr.S_un.S_un_b.s_b4, from.sin_port, buffer);
	}

	printf("done");

	WSACleanup();

	return 0;
}
