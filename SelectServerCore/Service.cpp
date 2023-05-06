#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "SelectServerCore.h"
#include "Listener.h"
/*-------------------
	Service
--------------------*/
Service::Service(ServiceType type, SelectServerCore* core, SessionFactory factory, int32 maxSessionCount)
	: m_type(type), m_selectCore(core), m_sessionFactory(factory), m_maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{
}

void Service::CloseService()
{
}

void Service::AddSession(Session* session)
{
	m_sessionCount++;
	m_sessions.insert(session);
}

void Service::ReleaseSession(Session* session)
{
	m_sessionCount--;
	m_sessions.erase(session);
}

Session* Service::CreateSession()
{
	return m_sessionFactory();
}

void Service::Work()
{
	m_selectCore->Dispatch();
}

/*-------------------
	ServerService
--------------------*/
ServerService::ServerService(SelectServerCore* core, SessionFactory factory, const char* ip, int32 port, int32 maxSessionCount)
	: Service(ServiceType::Server, core, factory, maxSessionCount)
{
	::memset(&m_sockAddr, 0, sizeof(m_sockAddr));
	m_sockAddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &m_sockAddr.sin_addr);
	m_sockAddr.sin_port = htons(port);
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	m_listener = new Listener();

	if (m_listener == nullptr)
		return false;

	if (m_listener->StartAccept(this) == false)
		return false;

	m_selectCore->Register(m_listener);

	return true;
}

void ServerService::CloseService()
{
	delete m_listener;
}