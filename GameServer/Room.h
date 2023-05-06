#pragma once
class Session;
class GameServerSession;
class SendBuffer;
class Player;
class Room
{
private:
	int							  m_roomSq;
	WCHAR						  m_title[20];
	std::set<GameServerSession*>  m_session;
	RoomStatus					  m_status = RoomStatus::HOLD;
	GameServerSession*			  m_regBy;
	Player*						  m_players[4] = { nullptr, nullptr, nullptr, nullptr };
	int32						  m_nextUserOrder = 0;
public:
	Room(WCHAR* title, int size, GameServerSession* regSession);
	~Room();

	int    GetRoomSQ() { return m_roomSq; }
	int	   GetSessionCount() { return m_session.size(); }
	WCHAR* GetTitle() { return m_title; }
	int32  GetRoomStatus() { return static_cast<int32>(m_status); }
	void   AddSession(GameServerSession* session);
	void   ReleaseSession(GameServerSession* session);
	WCHAR* GetRegBy();
	
	void   EnterPlayer(GameServerSession* session);
	void   BroadCast(SendBuffer* sendBuffer);
	void   OrderRefresh();
	void   PickChange(GameServerSession* session, BYTE* buffer, int32 len);
	bool   Ready(GameServerSession* session, BYTE* buffer, int32 len);
	std::set<GameServerSession*>& GetSessions() { return m_session; }
	void   SetStatus(RoomStatus status) { m_status = status; }
};

