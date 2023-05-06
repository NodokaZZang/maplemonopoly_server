#pragma once
#include "Session.h"
class Player;

class GameServerSession : public TestSession
{
private:
	Player* m_player;
public:
	~GameServerSession()
	{
	
	}
	static GameServerSession* MakeGameServerSession() 
	{
		return new GameServerSession();
	}

	virtual void		OnConnected();
	virtual void		OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void		OnSend(int32 len);
	virtual void		OnDisconnected();

	void				SetPlayer(Player* player) { m_player = player;}
	Player*				GetPlayer() { return m_player; }
};

