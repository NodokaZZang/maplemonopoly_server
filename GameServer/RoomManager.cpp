#include "pch.h"
#include "RoomManager.h"
#include "Room.h"
RoomManager GRoomManager;

void RoomManager::Add(int32 roomSq, Room* room)
{
	m_room.insert({ roomSq,room });
}

void RoomManager::Release(int32 roomSq)
{
	auto room = m_room.find(roomSq)->second;
	if (room != nullptr)
	{
		m_room.erase(roomSq);
		delete room;
	}
}

Room* RoomManager::GetRoom(int32 roomSq)
{
	auto room = m_room.find(roomSq)->second;
	
	return room;
}
