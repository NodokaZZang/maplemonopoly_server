#pragma once

class TempBuffer
{
private:
	std::vector<BYTE> m_buffer;
	int32 m_size = 0;
	int32 m_pos = 0;

public:
	TempBuffer(int32 size);
	~TempBuffer();

	BYTE* Data() { return m_buffer.data(); }
	void Write(BYTE* buffer, int32 size);
	void InitPos() { m_pos = 0; }

};

class CircleRecvBuffer
{
private:
	int32 m_readPos = 0;
	int32 m_writePos = 0;
	int32 m_size = 0;
	std::vector<BYTE> m_buffer;
	TempBuffer m_tmpBuffer;
public:
	CircleRecvBuffer(int32 size);
	~CircleRecvBuffer();

	BYTE* WritePos() { return &m_buffer[m_writePos]; }
	BYTE* ReadPos() { return &m_buffer[m_readPos]; }
	void  UpdateWritePos(int32 recvSize);
	void  UpdateReadPos(int32 processLen);
	int32 DataSize();
	int32 FreeSize();
	int32 OnRead(BYTE** dataPtr, int32* numOfByte);

};

