
//platform detection
#define PLATFORM_WINDOWS	1
#define PLATFORM_MAC		2
#define PLATFORM_UNIX		3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
#include <WinSock2.h>

#pragma comment(lib, "wsock32.lib")
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

//must call WSAStartup if using windows to accsess sockets
bool InitalizeSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
	WSADATA WsaData;
	return WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;
#else
	return true;
#endif
}

//must call WSACleanup to shutdown socket for windows
void ShutdownSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
	WSACleanup();
#endif
}



#include <stdio.h>

int main()
{
	//creates socket
	int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (handle <= 0)
	{
		printf("failed to create socket \n");
		return false;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	//0 is port  dont use numbers lower than 1024 an dont use numbers above 50000
	// ports 1024 and lower are used for systems
	// ports 50000 and above are used for dynamically assigning ports
	// use 0 if you want the system to use a free port
	addr.sin_port = htons((unsigned short)0);
	//htons is used to convert a 16 byte int value from host byte order to network byte order
	// htons stands for host to network short (16 bit)
	// htonl stands for host to network long (32 bit)


	char packet_data;
	int packet_size;

	//sending a packet
	
	//creats address of 207.45.186.98:30000
	unsigned int a = 207;
	unsigned int b = 45;
	unsigned int c = 186;
	unsigned int d = 98;
	unsigned short port = 30000;

	unsigned int address = (a << 24) | (b << 16) | (c << 8) | d;
	
	addr.sin_addr.s_addr = htonl(address);
	
	if (bind(handle, (const sockaddr*)&addr, sizeof(sockaddr_in)) < 0)
	{
		printf("failed to bind socket\n");
		return false;
	}

	int sent_bytes = sendto(handle, (const char*)packet_data, packet_size, 0, (sockaddr*)&addr,sizeof(sockaddr_in));
	if (sent_bytes != packet_size)
	{
		printf("falid to send packet\n");
		return false;
	}

	//will only process packets 256 and smaller
	// if recieve a packet that is 300 the entier packet will be droped
	//procces to recieve packets
	while (true)
	{
		unsigned char packet_data[256];

		unsigned int max_packet_size = sizeof(packet_data);

#if PLATFORM == PLATFORM_WINDOWS
		typedef int socklen_t;
#endif
		sockaddr_in from;
		socklen_t fromLength = sizeof(from);

		int bytes = recvfrom(handle, (char*)packet_data, max_packet_size, 0, (sockaddr*)&from, &fromLength);

		if (bytes <= 0)
			break;
		unsigned int from_address = ntohl(from.sin_addr.s_addr);
		unsigned int from_port = ntohs(from.sin_port);
	}
	
	return 0;
}

// non_blocking Socket
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

int nonBlocking = 1;
if (fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
{
	printf("failed to set non-blocking\n");
	return false;
}
//windows cant use fcntl so we have to use ioctlsocket
#elif PLATFORM == PLATFORM_WINDOWS
DWORD nonBlocking = 1;
if (ioctlsocket(handle, FIONBIO, &nonBlocking) != 0)
{
	printf("failed to set non-blocking\n");
	return false;
}

#endif

// close sockets
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
close(handle);
#elif PLATFORM == PLATFORM_WINDOWS
closesocket(handle);
#endif