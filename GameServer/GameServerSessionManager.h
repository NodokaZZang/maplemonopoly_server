#pragma once
class GameServerSession;
class SendBuffer;

class GameServerSessionManager
{
public:
	void Add(GameServerSession* session);
	void Remove(GameServerSession* session);
	void Broadcast(SendBuffer* sendBuffer);

private:
	std::set<GameServerSession*> m_sessions;
};

extern GameServerSessionManager GSessionManager;