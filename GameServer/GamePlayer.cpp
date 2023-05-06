#include "pch.h"
#include "GamePlayer.h"
#include "Map.h"
GamePlayer::GamePlayer(Pick _pick, int _money, int _pos, int _userId, int _order)
	: m_pick(_pick), m_money(_money), m_pos(_pos), m_userId(_userId), m_order(_order)
{
}

GamePlayer::~GamePlayer()
{
}

void GamePlayer::AddMoney(int _money)
{
	m_money += _money;
}

void GamePlayer::SubMoney(int _money)
{
	m_money -= _money;

	if (m_money <= 0)
		m_money = 0;
}

Region GamePlayer::Move(int _power, Map* _map)
{
	m_pos = (m_pos + _power);

	if (m_pos >= 32)
	{
		m_pos = m_pos % 32;
		m_salry = true;
	}

	Region region = _map->GetRegion(m_pos);
	return region;
}

bool GamePlayer::ISSalry()
{
	bool ret = m_salry;
	m_salry = false;
	return ret;
}

bool GamePlayer::ISISland()
{
	bool ret = m_islandCount > 0;
	m_islandCount--;

	if (m_islandCount <= 0)
		m_islandCount = 0;

	return ret;
}

void GamePlayer::SetIsland()
{
	m_islandCount = 2;
}
