#pragma once
#include <functional>
enum class ServiceType : uint8 
{
	Server,
	Client
};


class Session;
class SelectServerCore;
class Listener;

using SessionFactory = std::function<Session*(void)>;
/*-------------------
	Service
--------------------*/
class Service
{
protected:
	ServiceType			 m_type;
	SOCKADDR_IN			 m_sockAddr = {};
	SelectServerCore*	 m_selectCore;
	SessionFactory		 m_sessionFactory;
	int32				 m_maxSessionCount = 0;
	std::set<Session*>	 m_sessions;
	int32				 m_sessionCount = 0;

public:
	Service(ServiceType type, SelectServerCore* core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual bool		Start() abstract;
	bool				CanStart() { return m_sessionFactory != nullptr; }
	virtual void		CloseService();
	void				SetSessionFactory(SessionFactory func) { m_sessionFactory = func; }
	int32				GetCurrentSessionCount() { return m_sessionCount; }
	int32				GetMaxSessionCount() { return m_maxSessionCount; }
	SOCKADDR_IN&		GetSockADDR() { return m_sockAddr; }
	void				AddSession(Session* session);
	void				ReleaseSession(Session* session);
	Session*			CreateSession();
	void				Work();
};

/*-------------------
	ServerService
--------------------*/
class ServerService : public Service
{
private:
	Listener* m_listener = nullptr;

public:
	ServerService(SelectServerCore* core, SessionFactory factory, const char* ip, int32 port, int32 maxSessionCount = 1);
	virtual ~ServerService() { CloseService(); }

	virtual bool	Start() override;
	virtual void	CloseService() override;
};