#pragma once
class BufferOutputStream
{
private:
	BYTE* m_buffer;
	uint32	m_size = 0;
	uint32	m_pos = 0;

public:
	BufferOutputStream();
	BufferOutputStream(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferOutputStream();

	BYTE*			Buffer() { return m_buffer; }
	uint32			Size() { return m_size; }
	uint32			WriteSize() { return m_pos; }
	uint32			FreeSize() { return m_size - m_pos; }

	template<typename T>
	T* Reserve(uint16 count = 1);

	template<typename T>
	bool			Write(T* src) { return Write(src, sizeof(src)); }
	bool			Write(void* src, uint32 len);

	template<typename T>
	BufferOutputStream& operator<<(OUT T& dest);
};

template<typename T>
inline T* BufferOutputStream::Reserve(uint16 count)
{
	if (FreeSize() < (sizeof(T) * count))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&m_buffer[m_pos]);
	m_pos += sizeof(T) * count;
	return ret;
}

template<typename T>
inline BufferOutputStream& BufferOutputStream::operator<<(OUT T& dest)
{
	*(reinterpret_cast<T*>(&m_buffer[m_pos])) = dest;
	m_pos += sizeof(T);
	return *this;
}
