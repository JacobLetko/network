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

	char buffer[SOCKET_BUFFER_SIZE];
	INT32 player_x;
	INT32 player_y;

	printf("type w, a, s, or d to move, q to quit\n");
	bool is_running;
	while (is_running)
	{
		//get Input
		scanf_s("\n&c", &buffer[0], 1);
		 
		// send to server
		int buffer_length = 1;
		int flags = 0;
		SOCKADDR* to = (SOCKADDR*)&server_address;

		int to_length = sizeof(server_address);
		if (sendto(sock, buffer, buffer_length, flags, to, to_length) == SOCKET_ERROR)
		{
			printf("sendto failed: %d", WSAGetLastError());
			return;
		}

		// wait for reply
		flags = 0; 
		SOCKADDR_IN from;
		int from_size = sizeof(from);
		int bytes_recived = recvfrom(sock, buffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);

		if (bytes_recived == SOCKET_ERROR)
		{
			printf("recvfrom returned Socket_Error: %d", WSAGetLastError());
			break;
		}

		// grab data from packet
		INT32 read_index = 0;

		memcpy(&player_x, &buffer[read_index], sizeof(player_x));
		read_index += sizeof(player_x);

		memcpy(&player_y, &buffer[read_index], sizeof(player_y));
		read_index += sizeof(player_y);

		memcpy(&is_running, &buffer[read_index], sizeof(is_running));

		printf("x:%d, y:%d, is_running:%d\n", player_x, player_y, is_running);
	}

	return 0;
}
