#pragma once
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include "CircleRecvBuffer.h"
/*-------------------
	Session
--------------------*/
class Session
{
	friend class Listener;
	friend class SelectServerCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, // 64KB
	};

private:
	SOCKET				m_socket;
	CircleRecvBuffer	m_recvBuffer;
	SOCKADDR_IN			m_sockAddr = {};
	bool				m_connected;
public:
	Session();
	virtual ~Session();

	/* 외부에서 사용 */
	void				Send(SendBuffer* sendBuffer);
	bool				Recv();
	bool				Connect();
	void				Disconnect(const WCHAR* cause);

	/* 정보 관련 */
	void				SetAddress(SOCKADDR_IN& address) { m_sockAddr = address; }
	void				SetSocket(SOCKET& socket) { m_socket = socket; }
	SOCKADDR_IN			GetAddress() { return m_sockAddr; }
	SOCKET				GetSocket() { return m_socket; }
	bool				IsConnected() { return m_connected; }

public:
	/* 컨텐츠 코드에서 재정의 */
	virtual void		OnConnected() { }
	virtual int32		OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void		OnSend(int32 len) { }
	virtual void		OnDisconnected() { }
};

/*-------------------
	PacketSession
--------------------*/
struct PacketHeader
{
	uint16 size;
	uint16 id; // 프로토콜ID (ex. 1=로그인, 2=이동요청)
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

protected:
	virtual int32		OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void		OnRecvPacket(BYTE* buffer, int32 len) abstract;
};

class TestSession : public Session 
{
public:
	TestSession();
	virtual ~TestSession();

protected:
	virtual int32		OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void		OnRecvPacket(BYTE* buffer, int32 len) abstract;
};