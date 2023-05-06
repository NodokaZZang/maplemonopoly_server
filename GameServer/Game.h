#pragma once
class GamePlayer;
class Map;
class GameServerSession;
class SendBuffer;
class Game
{
private:
	int								m_roomSq = 0;
	int								m_turnPlayer = 0;
	std::set<GameServerSession*>	m_session;
	GamePlayer*						m_players[4] = { nullptr, nullptr, nullptr, nullptr };
	Map*							m_map;
public:
	Game();
	~Game();


	void						SetRoomSq(int32 roomSq) { m_roomSq = roomSq; }
	int32						GetRoomSq() { return m_roomSq; }
	void						AddSession(GameServerSession* session);
	void						ReleaseSession(GameServerSession* session);
	void						Init(GameServerSession* session);
	void						BroadCast(SendBuffer* sendBuffer);
	void						SendTurn();
	void						NextTurn();
	void						DiceDrop(GameServerSession* session, BYTE* buffer, int32 len);
	void						DiceDropEnd(GameServerSession* session, BYTE* buffer, int32 len);
	void						PlayerMoveEnd(GameServerSession* session, BYTE* buffer, int32 len);
	void						ProcessBuyRegionModalOpen(GameServerSession* session, Region region);
	void						ProcessBuyOtherRegionModalOpen(GameServerSession* session, Region region);
	void						ProcessPlayerDead(GameServerSession* session, int32 playerIndex, int32 owerIndex, int32 money);
	void						ProcessPlayerPayCost(GameServerSession* session, int32 playerIndex, int32 passAfterMoney, int32 ownerIndex, int32 passAfterMoneyOther);
	void						BuyRegion(GameServerSession* session, BYTE* buffer, int32 len);
	void						BuyOtherRegion(GameServerSession* session, BYTE* buffer, int32 len);
	bool						PlayerDead(GameServerSession* session, BYTE* buffer, int32 len);
	bool						PlayerDead(int32 playerIndex);
	int32						PlayerCount();

	int32						GetSessionCount() { return m_session.size(); }
	bool ISEnd();
	void PlayerDisconnect(int32 playerIndex);
};

