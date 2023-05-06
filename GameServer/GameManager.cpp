#include "pch.h"
#include "GameManager.h"

GameManager GGameManager;

void GameManager::Add(int32 roomSq, Game* room)
{
	m_game.insert({ roomSq, room });
}

void GameManager::Release(int32 roomSq)
{
	Game* game = m_game.find(roomSq)->second;
	delete game;
	m_game.erase(roomSq);
}
	
Game* GameManager::GetGame(int32 roomSq)
{
	if (m_game.find(roomSq) != m_game.end())
		return m_game.find(roomSq)->second;
	else
		return nullptr;
}
