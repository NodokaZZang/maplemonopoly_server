#pragma once
class Room;
class RoomManager
{
private:
	std::map<int32, Room*> m_room;

public:
	void				  Add(int32 roomSq, Room* room);
	void				  Release(int32 roomSq);
	Room*				  GetRoom(int32 roomSq);
	std::map<int, Room*>& GetRoomData() { return m_room; }
	int32				  GetRoomCount() { return m_room.size(); }
};

extern RoomManager GRoomManager;
