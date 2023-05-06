#include "pch.h"
#include "Room.h"
#include "GameServerSession.h"
#include "Player.h"
#include "RoomManager.h"
#include "BUfferOutputStream.h"
#include "Lobby.h"
#include "BufferInputStream.h"

Room::Room(WCHAR* title, int size, GameServerSession* regSession) : m_regBy(regSession)
{
	static int32 roomSq = 1;
	regSession->GetPlayer()->SetRoomSq(roomSq);
	m_roomSq = roomSq++;
	memcpy(m_title, title, size);
}

Room::~Room()
{

}

void Room::AddSession(GameServerSession* session)
{
	session->GetPlayer()->SetLocation(Location::WATING_ROOM);
	session->GetPlayer()->SetRoomSq(m_roomSq);

	OrderRefresh();
	m_players[m_nextUserOrder] = session->GetPlayer();
	session->GetPlayer()->SetOrder(m_nextUserOrder);
	EnterPlayer(session);
}

void Room::ReleaseSession(GameServerSession* session)
{
	session->GetPlayer()->SetLocation(Location::LOBBY_ROOM);
	session->GetPlayer()->SetRoomSq(0);
	session->GetPlayer()->SetReady(false);

	m_players[session->GetPlayer()->GetOrder()] = nullptr;
	session->GetPlayer()->SetOrder(0);
	m_session.erase(session);

	bool sendFlag = false;

	if (m_regBy == session)
	{
		if (m_session.size() == 0) 
		{
			GRoomManager.Release(m_roomSq);
		}
		else
		{
			for (auto& item : m_session) 
			{
				m_regBy = item;
				m_players[item->GetPlayer()->GetOrder()] = nullptr;
				item->GetPlayer()->SetOrder(0);
				m_players[0] = item->GetPlayer();
				sendFlag = true;
				break;
			}
		}
	}
	else
	{
		sendFlag = true;
	}

	if (sendFlag) 
	{
		SendBuffer sendBuffer(session, 2048);
		BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());
		PacketHeader* header = os.Reserve<PacketHeader>();

		int32 playerCount = m_session.size();
		os << playerCount;

		for (int i = 0; i < 4; i++) 
		{
			if (m_players[i] != nullptr) 
			{
				Player* p = m_players[i];

				int32 pick = p->GetPick();
				int32 order = p->GetOrder();
				bool ready = p->GetReady();
				int32 usernameSize = p->GetUsernameSize();
				WCHAR* username = p->GetUsername();

				os << pick << order << ready << usernameSize;
				os.Write(username, usernameSize);
			}
		}

		sendBuffer.Close(os.WriteSize());
		header->id = PKT_C_WROOM_ASYNCUSERLIST;
		header->size = os.WriteSize();
		BroadCast(&sendBuffer);
	}
}


WCHAR* Room::GetRegBy()
{
	return m_regBy->GetPlayer()->GetUsername();
}

void Room::EnterPlayer(GameServerSession* session)
{

	for (auto& s : m_session) 
	{
		SendBuffer sendBuffer(session, 2048);
		BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

		Player* enterPlayer = session->GetPlayer();
		
		PacketHeader* header = os.Reserve<PacketHeader>();

		int32 pick = enterPlayer->GetPick();
		int32 order = enterPlayer->GetOrder();
		bool ready = enterPlayer->GetReady();
		int32 usernameSize = enterPlayer->GetUsernameSize();
		WCHAR* username = enterPlayer->GetUsername();

		os << pick << order << ready << usernameSize;
		os.Write(username, usernameSize);

		sendBuffer.Close(os.WriteSize());

		header->id = PKT_C_WROOM_ENTEROTHERUSER;
		header->size = os.WriteSize();

		s->Send(&sendBuffer);
	}

	m_session.insert(session);

	SendBuffer sendBuffer(session, 4096);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());
	PacketHeader* header = os.Reserve<PacketHeader>();

	int32 playerCount = m_session.size();
	os << playerCount;
	
	for (auto& s : m_session) 
	{
		Player* otherPlayer = s->GetPlayer();
		
		int32 pick = otherPlayer->GetPick();
		int32 order = otherPlayer->GetOrder();
		bool ready = otherPlayer->GetReady();
		int32 usernameSize = otherPlayer->GetUsernameSize();
		WCHAR* username = otherPlayer->GetUsername();

		os << pick << order << ready << usernameSize;
		os.Write(username, usernameSize);
	}

	int32 titleSize = wcslen(m_title) * sizeof(WCHAR);
	WCHAR* title = m_title;

	os << titleSize;
	os.Write(title, titleSize);

	header->id = PKT_C_WROOM_ENTERUSER;
	header->size = os.WriteSize();

	sendBuffer.Close(os.WriteSize());
	session->Send(&sendBuffer);
}

void Room::BroadCast(SendBuffer* sendBuffer)
{
	for (auto& session : m_session) 
	{
		session->Send(sendBuffer);
	}
}

void Room::OrderRefresh()
{
	while (m_players[m_nextUserOrder] != nullptr)
	{
		m_nextUserOrder += 1;
		m_nextUserOrder = m_nextUserOrder % 4;
	}
}

void Room::PickChange(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);
	PacketHeader header;
	int32		 pick;

	in >> header >> pick;

	session->GetPlayer()->SetPick((Pick)pick);

	SendBuffer sendBuffer(session, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());
	PacketHeader* oheader = os.Reserve<PacketHeader>();
	int32 o_order = session->GetPlayer()->GetOrder();
	int32 o_pick = pick;

	os << o_order << o_pick;
	oheader->size = os.WriteSize();
	oheader->id = PKT_C_PICKCHANGE;
	sendBuffer.Close(os.WriteSize());
	
	BroadCast(&sendBuffer);
}

bool Room::Ready(GameServerSession* session, BYTE* buffer, int32 len)
{
	Player* p = session->GetPlayer();
	
	bool next = !p->GetReady();

	p->SetReady(next);
	
	if (session->GetPlayer()->GetOrder() == 0)
	{
		int cnt = 0;
		for (int i = 0; i < 4; i++) 
		{
			if (m_players[i] != nullptr && m_players[i]->GetReady() == true)
			{
				cnt++;
			}
		}

		if (cnt != 1 && cnt != 0 && cnt == m_session.size()) 
		{
			for (int i = 0; i < 4; i++)
				if (m_players[i])
					m_players[i]->SetReady(false);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		SendBuffer sendBuffer(session, 256);
		BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());
		PacketHeader* header = os.Reserve<PacketHeader>();
		int32 order = p->GetOrder();
		os << order << next;
		sendBuffer.Close(os.WriteSize());

		header->id = PKT_C_READY;
		header->size = os.WriteSize();

		BroadCast(&sendBuffer);
		m_status = RoomStatus::GAME;
		return false;
	}
}
