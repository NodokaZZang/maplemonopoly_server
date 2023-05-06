#include "pch.h"
#include "GameServerSession.h"
#include "GameServerSessionManager.h"
#include "ClientPacketHandler.h"
#include "Player.h"
void GameServerSession::OnConnected()
{
	GSessionManager.Add(this);
}

void GameServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	GClinetPacketHandler.HandlePacket(this, buffer, len);
}

void GameServerSession::OnSend(int32 len)
{
	// 사용 안함
}

void GameServerSession::OnDisconnected()
{
	GSessionManager.Remove(this);
	if (m_player)
		delete m_player;
}
