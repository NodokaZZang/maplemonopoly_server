#include "pch.h"
#include "TestRecvBuffer.h"
#include "Session.h"
TestRecvBuffer::TestRecvBuffer(int32 bufferSize, Session* owner) : m_size(bufferSize), m_owner(owner)
{
	m_buffer.resize(bufferSize);
}

TestRecvBuffer::~TestRecvBuffer()
{

}

int32 TestRecvBuffer::DataSize()
{
	if (m_readPos > m_writePos)
		return (m_size + m_writePos) - m_readPos;

	return m_writePos - m_readPos;
}

int32 TestRecvBuffer::FreeSize()
{
	int writePos = m_writePos;

	if (m_readPos > m_writePos)
	{
		return m_readPos - m_writePos;
	}
	else
	{
		return m_size - m_writePos;
	}
}

// [W][][][][R][]
bool TestRecvBuffer::OnRead()
{
	// [][R][][W]
	int32 recvSize = recv(m_owner->GetSocket(), reinterpret_cast<char*>(WritePos()), FreeSize(), 0);

	if (recvSize <= 0)
		return false;

	m_writePos += recvSize;
	m_writePos %= m_size;
	
	while (true)
	{
		int32 dataSize = DataSize();

		if (dataSize == 0)
			break;

		if (dataSize < sizeof(PacketHeader))
			break;
		
		if (m_readPos + (sizeof(PacketHeader) - 1) >= m_size)
		{
			BYTE tempBuffer[523];
			int32 index = 0;

			int32 lsize = m_size - m_readPos; // [][R][R]
			int32 rsize = sizeof(PacketHeader) - lsize;

			memcpy(tempBuffer + index, ReadPos(), lsize);
			index += lsize;
			memcpy(tempBuffer + index, &m_buffer[0], rsize);
			index += rsize;

			int32 headerSize = reinterpret_cast<PacketHeader*>(tempBuffer)->size;

			if (headerSize < dataSize)
				break;
			else
			{
				memcpy(tempBuffer + index, &m_buffer[rsize], headerSize - sizeof(PacketHeader));
				index += headerSize - sizeof(PacketHeader);

				m_owner->OnRecv(tempBuffer, headerSize);
				
				m_readPos += headerSize;
				m_readPos %= m_size;
			}

		}
		else
		{
			int32 pktSize = reinterpret_cast<PacketHeader*>(ReadPos())->size;

			if (dataSize < pktSize)
				break;

			if (m_readPos + pktSize - 1 >= m_size)
			{
				BYTE tempBuffer[523];
				int32 index = 0;

				memcpy(tempBuffer + index, ReadPos(), m_size - m_readPos);
				index = m_size - m_readPos;
				memcpy(tempBuffer + index, &m_buffer[0], m_writePos);
				m_owner->OnRecv(tempBuffer, pktSize);
				m_readPos += pktSize;
				m_readPos %= m_size;
			}

			else
			{
				m_owner->OnRecv(ReadPos(), pktSize); // 패킷 하나만 처리하는 세션
				m_readPos += pktSize;
				m_readPos %= m_size;
			}
		}
	}

	return true;
}
