#include "pch.h"
#include "ClientPacketHandler.h"
#include "GameServerSession.h"
#include "BufferOutputStream.h"
#include "BufferInputStream.h"
#include "Player.h"
#include "GameServerSessionManager.h"
#include "Lobby.h"
#include "Room.h"
#include "RoomManager.h"
#include "GameManager.h"
#include "Game.h"
ClientPacketHandler GClinetPacketHandler;

void ClientPacketHandler::HandlePacket(GameServerSession* session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	switch (header->id)
	{
	case PKT_S_LOGIN:
		HandlePacket_LOGIN(session, buffer, len);
		break;

	case PKT_S_LOBBYASYNC:
		HandlePacket_LOBBYASYNC(session, buffer, len);
		break;

	case PKT_S_LOBBYCHAT:
		HandlePacket_LOBBYCHAT(session, buffer, len);
		break;

	case PKT_S_CREATEROOM:
		HandlePacket_CREATEROOM(session, buffer, len);
		break;

	case PKT_S_ENTERROOM:
		HandlePacket_ENTERROOM(session, buffer, len);
		break;

	case PKT_S_WROOMCHAT:
		HandlePacket_WROOMCHAT(session, buffer, len);
		break;

	case PKT_S_EXITWROOM:
		HandlePacket_WROOMEXIT(session, buffer, len);
		break;

	case PKT_S_PICKCHANGE:
		HandlePacket_WROOMPICKCHANGE(session, buffer, len);
		break;

	case PKT_S_READY:
		HandlePacket_WROOMREADY(session, buffer, len);
		break;

	case PKT_S_DICEDROP:
		HandlePacket_DICEDROP(session, buffer, len);
		break;

	case PKT_S_DICEDROPEND:
		HandlePacket_DICEDROPEND(session, buffer, len);
		break;

	case PKT_S_PLAYERMOVEEND:
		HandlePacket_PLAYERMOVEEND(session, buffer, len);
		break;

	case PKT_S_PASSTURN:
		HandlePacket_PASSTURN(session, buffer, len);
		break;

	case PKT_S_BUYOTHERREGION:
		HandlePacket_BUYOTHERREGION(session, buffer, len);
		break;

	case PKT_S_BUYREGION:
		HandlePacket_BUYREGION(session, buffer, len);
		break;

	case PKT_S_PLAYERDEAD:
		HandlePacket_PLAYERDEAD(session, buffer, len);
		break;
	}
}

void ClientPacketHandler::HandlePacket_LOGIN(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);

	PacketHeader header;
	int32 strSize;
	WCHAR username[15] = {};

	in >> header >> strSize;
	in.Read(username,strSize);

	Player* player = new Player(username);
	session->SetPlayer(player);
	GLobby.AddSession(session);
}

void ClientPacketHandler::HandlePacket_LOBBYASYNC(GameServerSession* session, BYTE* buffer, int32 len)
{
	GLobby.AsyncData(session);
}

void ClientPacketHandler::HandlePacket_LOBBYCHAT(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);

	PacketHeader header;
	int32 textSize;
	WCHAR text[15] = {};
	WCHAR str[30] = {};

	in >> header >> textSize;
	in.Read(text, textSize);

	Player* player = session->GetPlayer();
	swprintf_s(str, _countof(str), L"%s:%s", player->GetUsername(), text);

	SendBuffer sendBuffer(session, 4096);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

	PacketHeader* oheader = os.Reserve<PacketHeader>();

	int32 strSize = wcslen(str) * sizeof(WCHAR);
	os << strSize;
	os.Write(str, strSize);

	oheader->id = PKT_C_LOBBYCHAT;
	oheader->size = os.WriteSize();

	sendBuffer.Close(os.WriteSize());
	GLobby.BroadCast(&sendBuffer);
}

void ClientPacketHandler::HandlePacket_CREATEROOM(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);
	
	PacketHeader header;
	int32 textSize;
	WCHAR text[20] = {};

	in >> header >> textSize;
	in.Read(text, textSize);
	
	Room* newRoom = new Room(text, textSize, session);
	newRoom->AddSession(session);
	GRoomManager.Add(newRoom->GetRoomSQ(), newRoom);
}

void ClientPacketHandler::HandlePacket_ENTERROOM(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);

	PacketHeader header;
	int32 roomSq;
	in >> header >> roomSq;
	GRoomManager.GetRoom(roomSq)->AddSession(session);
}

void ClientPacketHandler::HandlePacket_WROOMCHAT(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);

	PacketHeader header;
	int32 textSize;
	WCHAR text[15] = {};
	WCHAR str[30] = {};

	in >> header >> textSize;
	in.Read(text, textSize);

	Player* player = session->GetPlayer();
	int32 roomSq = player->GetRoomSq();

	swprintf_s(str, _countof(str), L"%s:%s", player->GetUsername(), text);

	SendBuffer sendBuffer(session, 4096);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

	PacketHeader* oheader = os.Reserve<PacketHeader>();

	int32 strSize = wcslen(str) * sizeof(WCHAR);
	os << strSize;
	os.Write(str, strSize);

	oheader->id = PKT_C_WROOMCHAT;
	oheader->size = os.WriteSize();

	sendBuffer.Close(os.WriteSize());
	GRoomManager.GetRoom(roomSq)->BroadCast(&sendBuffer);
}

void ClientPacketHandler::HandlePacket_WROOMEXIT(GameServerSession* session, BYTE* buffer, int32 len)
{
	int32 roomSq = session->GetPlayer()->GetRoomSq();
	GRoomManager.GetRoom(roomSq)->ReleaseSession(session);
}

void ClientPacketHandler::HandlePacket_WROOMPICKCHANGE(GameServerSession* session, BYTE* buffer, int32 len)
{
	int32 roomSq = session->GetPlayer()->GetRoomSq();
	GRoomManager.GetRoom(roomSq)->PickChange(session, buffer, len);
}

void ClientPacketHandler::HandlePacket_WROOMREADY(GameServerSession* session, BYTE* buffer, int32 len)
{
	int32 roomSq = session->GetPlayer()->GetRoomSq();
	
	if (GRoomManager.GetRoom(roomSq)->Ready(session, buffer, len))
	{
		Game* game = new Game();
		game->SetRoomSq(roomSq);

		for (auto& s : GRoomManager.GetRoom(roomSq)->GetSessions())
		{
			game->AddSession(s);
		}
		GGameManager.Add(roomSq, game);

		SendBuffer sendBuffer(session, 256);
		BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

		PacketHeader* oheader = os.Reserve<PacketHeader>();

		oheader->id = PKT_C_ENTERGAME;
		oheader->size = os.WriteSize();
		sendBuffer.Close(os.WriteSize());
		GRoomManager.GetRoom(roomSq)->BroadCast(&sendBuffer);

		game->Init(session);
	}
}

void ClientPacketHandler::HandlePacket_DICEDROP(GameServerSession* session, BYTE* buffer, int32 len)
{
	int32 roomSq = session->GetPlayer()->GetRoomSq();

	GGameManager.GetGame(roomSq)->DiceDrop(session,buffer, len);
}

void ClientPacketHandler::HandlePacket_PLAYERMOVEEND(GameServerSession* session, BYTE* buffer, int32 len)
{
	int32 roomSq = session->GetPlayer()->GetRoomSq();

	GGameManager.GetGame(roomSq)->PlayerMoveEnd(session, buffer, len);
}

void ClientPacketHandler::HandlePacket_PASSTURN(GameServerSession* session, BYTE* buffer, int32 len)
{
	int32 roomSq = session->GetPlayer()->GetRoomSq();

	GGameManager.GetGame(roomSq)->NextTurn();
	GGameManager.GetGame(roomSq)->SendTurn();
}

void ClientPacketHandler::HandlePacket_BUYREGION(GameServerSession* session, BYTE* buffer, int32 len)
{
	int32 roomSq = session->GetPlayer()->GetRoomSq();
	GGameManager.GetGame(roomSq)->BuyRegion(session, buffer, len);
}

void ClientPacketHandler::HandlePacket_BUYOTHERREGION(GameServerSession* session, BYTE* buffer, int32 len)
{
	int32 roomSq = session->GetPlayer()->GetRoomSq();
	GGameManager.GetGame(roomSq)->BuyOtherRegion(session, buffer, len);
}

void ClientPacketHandler::HandlePacket_DICEDROPEND(GameServerSession* session, BYTE* buffer, int32 len)
{
	int32 roomSq = session->GetPlayer()->GetRoomSq();
	GGameManager.GetGame(roomSq)->DiceDropEnd(session, buffer, len);
}

void ClientPacketHandler::HandlePacket_PLAYERDEAD(GameServerSession* session, BYTE* buffer, int32 len)
{
	int32 roomSq = session->GetPlayer()->GetRoomSq();
	if (GGameManager.GetGame(roomSq)->PlayerDead(session, buffer, len)) 
	{
		GGameManager.Release(roomSq);
		GRoomManager.GetRoom(roomSq)->SetStatus(RoomStatus::HOLD);
	}
}
