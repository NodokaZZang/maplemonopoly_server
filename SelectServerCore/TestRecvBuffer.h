#pragma once
class Session;

class TestRecvBuffer
{
private:
	int32			  m_readPos = 0;
	int32			  m_writePos = 0;
	std::vector<BYTE> m_buffer;
	int32			  m_size = 0;
	Session*		  m_owner;
public:
	TestRecvBuffer(int32 bufferSize, Session* owner);
	~TestRecvBuffer();

	BYTE*			ReadPos() { return &m_buffer[m_readPos]; }
	BYTE*			WritePos() { return &m_buffer[m_writePos]; }
	int32			DataSize();
	int32			FreeSize();
	bool			OnRead();

};

