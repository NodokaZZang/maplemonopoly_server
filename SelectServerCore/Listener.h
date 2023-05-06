#pragma once
class ServerService;
class Session;
class Listener
{
private:
	SOCKET			m_socket;
	ServerService*  m_service;
public:
	Listener() = default;
	~Listener();

	bool			StartAccept(ServerService* service);
	Session*		ProcessAccept();
	SOCKET			GetSocket() { return m_socket; }
};

