#pragma once
class BufferInputStream
{
private:
	BYTE*	m_buffer;
	uint32	m_size = 0;
	uint32	m_pos = 0;

public:
	BufferInputStream();
	BufferInputStream(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferInputStream();

	BYTE*			Buffer() { return m_buffer; }
	uint32			Size() {  return m_size; }
	uint32			ReadSize() { return m_pos; }
	uint32			FreeSize() { return m_size - m_pos; }

	template<typename T>
	bool			Peak(T* dest) { return Peak(T, sizeof(dest)); }
	bool			Peak(void* dest, uint32 len);
	template<typename T>
	bool			Read(T* dest) { return Read(T, sizeof(dest)); }
	bool			Read(void* dest, uint32 len);

	template<typename T>
	BufferInputStream& operator>>(OUT T& dest);

};
// [d][d][d] [s]
template<typename T>
inline BufferInputStream& BufferInputStream::operator>>(OUT T& dest)
{
	dest = *reinterpret_cast<T*>(&m_buffer[m_pos]);
	m_pos += sizeof(T);
	return *this;
}
