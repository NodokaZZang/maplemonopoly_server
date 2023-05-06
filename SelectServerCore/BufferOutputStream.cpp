#include "pch.h"
#include "BufferOutputStream.h"

BufferOutputStream::BufferOutputStream()
{
}

BufferOutputStream::BufferOutputStream(BYTE* buffer, uint32 size, uint32 pos)
    : m_buffer(buffer), m_size(size), m_pos(pos)
{
}

BufferOutputStream::~BufferOutputStream()
{
}

bool BufferOutputStream::Write(void* src, uint32 len)
{
    if (FreeSize() <= len)
        return false;

    memcpy(&m_buffer[m_pos], src, len);
    m_pos += len;
    return true;
}
