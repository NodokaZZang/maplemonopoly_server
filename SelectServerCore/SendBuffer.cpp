#include "pch.h"
#include "SendBuffer.h"
#include "Session.h"

SendBuffer::SendBuffer(Session* owner,  uint32 allocSize): m_owner(owner), m_allocSize(allocSize)
{
	Open(allocSize);
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Open(uint32 allocSize)
{
	m_buffer.resize(allocSize);
}

void SendBuffer::Close(uint32 writeSize)
{
	m_writeSize += writeSize;
}
