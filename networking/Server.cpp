#include "loki.h"
#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h> // must be included after winsock2.h



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
		return;
	}

	char buffer[SOCKET_BUFFER_SIZE];
	INT32 player_x = 0;
	INT32 player_y = 0;
	bool is_running = 1;

	while (is_running)
	{
		// get packet from player
		int flags = 0;
		SOCKADDR_IN from;
		int from_size = sizeof(from);
		int bytes_recived = recvfrom(sock, buffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);
		if (bytes_recived == SOCKET_ERROR)
		{
			printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d", WSAGetLastError());
		}
		
		// process input
		char client_input = buffer[0];
		printf("%d.%d.%d.%d:%d - %c\n", from.sin_addr.S_un.S_un_b.s_b1, from.sin_addr.S_un.S_un_b.s_b2, from.sin_addr.S_un.S_un_b.s_b3, from.sin_addr.S_un.S_un_b.s_b4, from.sin_port, client_input);

		switch (client_input)
		{
		case 'w':
			++player_y;
			break;
		case 'a':
			--player_x;
			break;
		case 's':
				--player_y;
				break;
		case 'd':
			++player_x;
			break;
		case 'q':
			is_running = 0;
			break;
		default:
			printf("unhandled input %c\n", client_input);
			break;

		}

		// create state pack
		INT32 write_index = 0;
		memcpy(&buffer[write_index], &player_y, sizeof(player_x));
		write_index += sizeof(player_x);

		memcpy(&buffer[write_index], &is_running, sizeof(is_running));

		// send packet to client
		int buffer_length = sizeof(player_x) + sizeof(player_y) + sizeof(is_running);
		flags = 0;
		SOCKADDR* to = (SOCKADDR*)&from;
		int to_length = sizeof(from);
		if (sendto(sock, buffer, buffer_length, flags, to, to_length) == SOCKET_ERROR)
		{
			printf("sendto failed: &d", WSAGetLastError);
			return;
		}
	}

	printf("done");

	return 0;
}
