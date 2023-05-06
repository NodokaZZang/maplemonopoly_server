#include "pch.h"
#include "Player.h"

Player::Player(WCHAR* username)
{
	static int32 playerId = 1;
	m_userId = playerId++;
	wcscpy_s(m_username, username);
}
