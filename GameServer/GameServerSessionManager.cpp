#include "pch.h"
#include "GameServerSessionManager.h"
#include "GameServerSession.h"
#include "SendBuffer.h"
#include "Lobby.h"
#include "Player.h"
#include "RoomManager.h"
#include "Room.h"
#include "GameManager.h"
#include "Game.h"
GameServerSessionManager GSessionManager;

void GameServerSessionManager::Add(GameServerSession* session)
{
	m_sessions.insert(session);
}

void GameServerSessionManager::Remove(GameServerSession* session)
{
	m_sessions.erase(session);

	if (session->GetPlayer())
	{
		GLobby.ReleaseSession(session);
		if (session->GetPlayer())
		{
			Player* player = session->GetPlayer();
			if (player->GetRoomSq() != 0)
			{
				Game* game = GGameManager.GetGame(player->GetRoomSq());

				if (game != nullptr) 
				{
					game->ReleaseSession(session);
					if (game->GetSessionCount() == 0)
					{
						GGameManager.Release(player->GetRoomSq());
					}
					else
					{
						if (game->ISEnd())
						{
							if (game->PlayerDead(player->GetOrder()))
							{
								GGameManager.Release(player->GetRoomSq());
								GRoomManager.GetRoom(player->GetRoomSq())->SetStatus(RoomStatus::HOLD);
							}
						}
						else
						{
							game->PlayerDisconnect(player->GetOrder());
						}
					}
				}

				Room* room = GRoomManager.GetRoom(player->GetRoomSq());
				room->ReleaseSession(session);
			}	
		}
	}
}

void GameServerSessionManager::Broadcast(SendBuffer* sendBuffer)
{
	for (GameServerSession* session : m_sessions)
	{
		session->Send(sendBuffer);
	}
}
