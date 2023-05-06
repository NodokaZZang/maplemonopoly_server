#include "pch.h"
#include "RecvBuffer.h"
#include "Session.h"
RecvBuffer::RecvBuffer(int32 bufferSize, Session* owner) : m_bufferSize(bufferSize)
{
    m_capacity = bufferSize * BUFFER_COUNT;
    m_buffer.resize(m_capacity);
    m_owner = owner;
}

RecvBuffer::~RecvBuffer()
{
}

// [R][][][] [W][][][] [][][][]
bool RecvBuffer::OnRead()
{
    int32 recvSize = recv(m_owner->GetSocket(), reinterpret_cast<char*>(WritePos()), FreeSize(), 0);

    if (recvSize <= 0)
        return false;

    m_writePos += recvSize;

    // ÄÜÅÙÃ÷ ÄÚµå¿¡¼­ ÀçÁ¤ÀÇ
    int32 processLen = m_owner->OnRecv(ReadPos(), DataSize());

    if (processLen > DataSize())
        return false;

    m_readPos += processLen;

    int32 dataSize = DataSize();

    if (dataSize == 0)
    {
        m_readPos = m_writePos = 0;
    }
    else 
    {
        if (FreeSize() < m_bufferSize)
        {
            memcpy(m_buffer.data(), ReadPos(), DataSize());
            m_readPos = 0;
            m_writePos = DataSize();
        }
    }
    return true;
}