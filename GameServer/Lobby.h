#pragma once
#include "GameServerSession.h"
class Lobby
{
private:
	std::set<GameServerSession*> m_sessions;

public:
	void AddSession(GameServerSession* _session);
	void ReleaseSession(GameServerSession* _session);
	bool ContainSession(GameServerSession* _session);
	void AsyncData(GameServerSession* session);
	void BroadCast(SendBuffer* sendBuffer);
};

extern Lobby GLobby;