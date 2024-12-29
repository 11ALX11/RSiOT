#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

#include <iostream>
#include <WinSock2.h>
#include <winsock.h>

class ServerClientHandler
{
public:
	ServerClientHandler(SOCKET s, SOCKADDR_IN sock_in);
	~ServerClientHandler();
	void handle();
private:
	SOCKET c_sock;
	SOCKADDR_IN c_addr;
	char buffer[1024];
};

ServerClientHandler::ServerClientHandler(SOCKET s, SOCKADDR_IN sock_in)
{

	c_sock = s;
	c_addr = sock_in;
	printf("Client created\n");
	handle();
}


ServerClientHandler::~ServerClientHandler()
{
}

void ServerClientHandler::handle()
{
	while (true)
	{
		int k = recv(c_sock, buffer, sizeof(buffer), NULL);
		if (k > 0) {
			printf(buffer);
		}
		Sleep(30);
	}
}