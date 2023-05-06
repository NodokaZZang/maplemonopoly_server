#include "pch.h"
#include "Lobby.h"
#include "BufferOutputStream.h"
#include "BufferInputStream.h"
#include "Player.h"
#include "GameServerSessionManager.h"
#include "RoomManager.h"
#include "Room.h"
Lobby GLobby;

void Lobby::AddSession(GameServerSession* _session)
{
	m_sessions.insert(_session);
}

void Lobby::ReleaseSession(GameServerSession* _session)
{
	if (m_sessions.count(_session))
		m_sessions.erase(_session);
}

bool Lobby::ContainSession(GameServerSession* _session)
{
	return m_sessions.count(_session) != 0;
}

void Lobby::AsyncData(GameServerSession* session)
{
	SendBuffer sendBuffer(session, 4096);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

	PacketHeader* header = os.Reserve<PacketHeader>();

	uint32 playerCount = m_sessions.size();

	os << playerCount;

	for (auto& session : m_sessions) 
	{
		Player* player = session->GetPlayer();
		
		int32 usernameSize = player->GetUsernameSize();
		int32 location = player->GetLocation();
		
		os << location;
		os << usernameSize;
		os.Write(player->GetUsername(), usernameSize);
	}
	
	int32 playerNameSize = session->GetPlayer()->GetUsernameSize();
	os << playerNameSize;
	os.Write(session->GetPlayer()->GetUsername(), playerNameSize);
	
	int32 roomCount = GRoomManager.GetRoomCount();
	os << roomCount;

	for (auto item : GRoomManager.GetRoomData()) 
	{
		Room* room = item.second;
		
		int32 roomSq = room->GetRoomSQ();
		int32 roomStatus = room->GetRoomStatus();
		int32 roomPlayerCount = room->GetSessionCount();

		os << roomSq << roomStatus << roomPlayerCount;

		// 规力格
		int32 titleSize = wcslen(room->GetTitle()) * sizeof(WCHAR);
		os << titleSize;
		os.Write(room->GetTitle(), titleSize);

		// 累己磊
		int32 regByStrSize = wcslen(room->GetRegBy()) * sizeof(WCHAR);
		os << regByStrSize;
		os.Write(room->GetRegBy(), regByStrSize);
	}
	
	sendBuffer.Close(os.WriteSize());

	header->id = PKT_C_LOBBYASYNC;
	header->size = sendBuffer.WriteSize();

	session->Send(&sendBuffer);
}

void Lobby::BroadCast(SendBuffer* sendBuffer)
{
	for (auto& session : m_sessions)
	{
		session->Send(sendBuffer);
	}
}
