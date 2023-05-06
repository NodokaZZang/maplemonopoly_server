#include "pch.h"
#include "Service.h"
#include "SelectServerCore.h"
#include "GameServerSession.h"
int main() 
{
	srand(time(nullptr));

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), OUT & wsaData);

	SelectServerCore selectCore;

	ServerService* service 
		= new ServerService(
		      &selectCore
			, GameServerSession::MakeGameServerSession
			, "127.0.0.1"
			, 7777);

	if (service->Start())
		service->Work();
	else
		std::cout << "Server Can't Start" << std::endl;

	delete service;

	WSACleanup();
}