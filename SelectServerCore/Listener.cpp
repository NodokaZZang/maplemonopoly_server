#include "pch.h"
#include "Listener.h"
#include "Service.h"
#include "Session.h"
Listener::~Listener()
{
	::closesocket(m_socket);
}

bool Listener::StartAccept(ServerService* service)
{
	m_service = service;
	
	if (service == nullptr)
		return false;

	m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_socket == INVALID_SOCKET)
	{
		int errcode = WSAGetLastError();
		return false;
	}

	SOCKADDR_IN& sockAddrRef = m_service->GetSockADDR();

	if (bind(m_socket, (SOCKADDR*)&sockAddrRef, sizeof(sockAddrRef)) == SOCKET_ERROR)
		return false;

	if (listen(m_socket, 5) == SOCKET_ERROR)
		return false;

	return true;
}

Session* Listener::ProcessAccept()
{
	SOCKADDR_IN tempSockAddr;
	int addrSize = sizeof(tempSockAddr);
	SOCKET socketClient = accept(m_socket, (SOCKADDR*)&tempSockAddr, &addrSize);
	
	if (socketClient == SOCKET_ERROR)
		return nullptr;

	Session* session = m_service->CreateSession();

	session->SetSocket(socketClient);
	session->SetAddress(tempSockAddr);
	session->Connect();

	return session;
}
