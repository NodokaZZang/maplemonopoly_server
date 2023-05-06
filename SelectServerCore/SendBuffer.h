#pragma once
class Session;
class SendBuffer
{
private:
	uint32				m_allocSize = 0;
	uint32				m_writeSize = 0;
	Session*			m_owner;
	std::vector<BYTE>   m_buffer;
public:
	SendBuffer(Session* owner, uint32 allocSize);
	~SendBuffer();

	void		Open(uint32 allocSize);
	BYTE*		Buffer() { return m_buffer.data(); }
	uint32		AllocSize() { return m_allocSize; }
	uint32		WriteSize() { return m_writeSize; }
	void		Close(uint32 writeSize);
};