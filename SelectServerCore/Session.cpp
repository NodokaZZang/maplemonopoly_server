#include "pch.h"
#include "Session.h"
#include "RecvBuffer.h"
/*-------------------
	Session
--------------------*/
Session::Session() : m_recvBuffer(299)
{
	m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

Session::~Session()
{
	::closesocket(m_socket);
}

void Session::Send(SendBuffer* sendBuffer)
{
	::send(m_socket, reinterpret_cast<char*>(sendBuffer->Buffer()), sendBuffer->WriteSize(), 0);
}

bool Session::Recv()
{
	int32 recvSize = recv(m_socket, reinterpret_cast<char*>(m_recvBuffer.WritePos()), m_recvBuffer.FreeSize(), 0);

	if (recvSize <= 0)
		return false; 

	m_recvBuffer.UpdateWritePos(recvSize);

	while (true) 
	{
		BYTE* dataPtr = nullptr;
		int32 numOfBytes = 0;

		int32 processLen = m_recvBuffer.OnRead(&dataPtr,&numOfBytes);

		if (processLen == 0)
			break;

		m_recvBuffer.UpdateReadPos(processLen);

		if (dataPtr)
			OnRecv(dataPtr, numOfBytes);
	}

	return true;
}

bool Session::Connect()
{
	m_connected = true;

	// ÄÁÅÙÃ÷ ÄÚµå¿¡¼­ »ç¿ë
	OnConnected();

	return true;
}

void Session::Disconnect(const WCHAR* cause)
{
}

/*-------------------
	PacketSession
--------------------*/
PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (true) 
	{
		int32 dataSize = len - processLen;

		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(buffer));
		
		if (dataSize < header.size)
			break;

		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}

TestSession::TestSession()
{
}

TestSession::~TestSession()
{
}

int32 TestSession::OnRecv(BYTE* buffer, int32 len)
{
	OnRecvPacket(buffer, len);
	return 0;
}
