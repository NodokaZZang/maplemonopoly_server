#include "pch.h"
#include "BufferInputStream.h"

BufferInputStream::BufferInputStream()
{
}

BufferInputStream::BufferInputStream(BYTE* buffer, uint32 size, uint32 pos) : m_buffer(buffer), m_size(size), m_pos(pos)
{
}

BufferInputStream::~BufferInputStream()
{
}

bool BufferInputStream::Peak(void* dest, uint32 len)
{
	if (FreeSize() < len)
		return false;

	memcpy(dest, &m_buffer[m_pos], len);
	return true;
}

bool BufferInputStream::Read(void* dest, uint32 len)
{
	if (Peak(dest, len) == false)
		return false;

	m_pos += len;
	return true;
}
