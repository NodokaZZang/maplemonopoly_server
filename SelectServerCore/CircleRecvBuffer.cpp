#include "pch.h"
#include "CircleRecvBuffer.h"
#include "Session.h"
CircleRecvBuffer::CircleRecvBuffer(int32 size) : m_size(size), m_tmpBuffer(size)
{
	m_buffer.resize(size);
}

CircleRecvBuffer::~CircleRecvBuffer()
{
}

void CircleRecvBuffer::UpdateWritePos(int32 recvSize)
{
	m_writePos += recvSize;
	m_writePos %= m_size;
}

void CircleRecvBuffer::UpdateReadPos(int32 processLen)
{
	m_readPos += processLen;
	m_readPos %= m_size;
}

int32 CircleRecvBuffer::DataSize()
{
	if (m_readPos > m_writePos)
		return (m_size + m_writePos) - m_readPos;

	return m_writePos - m_readPos;
}

int32 CircleRecvBuffer::FreeSize()
{
	int32 writePos = m_writePos;
	
	if (m_readPos > m_writePos)
	{
		return m_readPos - m_writePos;
	}
	else
	{
		return m_size - m_writePos;
	}
}

int32 CircleRecvBuffer::OnRead(BYTE** dataPtr, int32* numOfByte)
{
	int32 dataSize = DataSize();
	int32 processLen = 0;

	if (dataSize == 0 || dataSize < sizeof(PacketHeader))
		return processLen;

	// Header°¡ Â©·ÈÀ»¶§
	if (m_readPos + sizeof(PacketHeader) - 1 >= m_size) 
	{
		int32 rightData = m_size - m_readPos; // [][R][R]
		int32 leftData = sizeof(PacketHeader) - rightData;
		// [0][][]
		m_tmpBuffer.Write(ReadPos(), rightData);
		m_tmpBuffer.Write(&m_buffer[0], leftData);
	
		int32 headerSize = reinterpret_cast<PacketHeader*>(m_tmpBuffer.Data())->size;

		if (dataSize >= headerSize)
		{
			m_tmpBuffer.Write(&m_buffer[leftData], headerSize - sizeof(PacketHeader));
			*dataPtr = m_tmpBuffer.Data();
			*numOfByte = headerSize;
			processLen = headerSize;
		}

		m_tmpBuffer.InitPos();
	}
	else
	{
		int32 pktSize = reinterpret_cast<PacketHeader*>(ReadPos())->size;
		
		if (dataSize < pktSize)
			return processLen;
		
		// [D][w][][R / D][ D]
		// µ¥ÀÌÅÍ°¡ Â©·ÈÀ» ¶§
		if (m_readPos + pktSize - 1 >= m_size)
		{
			m_tmpBuffer.Write(ReadPos(), m_size - m_readPos);
			m_tmpBuffer.Write(&m_buffer[0], m_writePos);
			
			*dataPtr = m_tmpBuffer.Data();
			*numOfByte = pktSize;
			processLen = pktSize;

			m_tmpBuffer.InitPos();
		}
		else 
		{
			*dataPtr = ReadPos();
			*numOfByte = pktSize;
			processLen = pktSize;
		}
	}

	return processLen;
}

TempBuffer::TempBuffer(int32 size) : m_size(size)
{
	m_buffer.resize(size);
}

TempBuffer::~TempBuffer()
{

}

void TempBuffer::Write(BYTE* buffer, int32 size)
{
	memcpy(&m_buffer[m_pos], buffer, size);
	m_pos += size;
}
