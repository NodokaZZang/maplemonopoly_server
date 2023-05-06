#pragma once
class Player
{
private:
	uint32		 m_userId = 0;
	uint32		 m_roomSq = 0;
	bool		 m_ready = false;
	WCHAR		 m_username[15] = {};
	Location     m_location = Location::LOBBY_ROOM;
	Pick		 m_pick = Pick::HORN_MURSHROOM;
	uint32		 m_order = 0;

public:
	Player() {};
	Player(WCHAR* username);
	~Player() {};

	int32		GetPick() { return static_cast<int32>(m_pick); }
	void		SetLocation(Location _location) { m_location = _location; }
	int32		GetUsernameSize() { return wcslen(m_username) * sizeof(WCHAR); }
	int32       GetLocation() { return static_cast<int32>(m_location); }
	WCHAR*		GetUsername() { return m_username; }
	int32		GetRoomSq()	  { return m_roomSq; }
	void		SetRoomSq(int32 roomSq) { m_roomSq = roomSq; }
	bool		GetReady() { return m_ready; }
	int32		GetOrder() { return m_order; }
	void		SetOrder(int order) { m_order = order; }
	void		SetPick(Pick pick) { m_pick = pick; }
	void		SetReady(bool ready) { m_ready = ready; }
	int32		GetUserId() { return m_userId; }
};

