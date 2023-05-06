#include "pch.h"
#include "Game.h"
#include "GameServerSession.h"
#include "Player.h"
#include "GamePlayer.h"
#include "BufferOutputStream.h"
#include "SendBuffer.h"
#include "BufferInputStream.h"
#include "Map.h"
Game::Game()
{
	m_map = new Map();
}
Game::~Game()
{
	if (m_map)
		delete m_map;
}
void Game::AddSession(GameServerSession* session)
{
	Player* player = session->GetPlayer();
	player->SetLocation(Location::GAME_ROOM);
	int orderIndex = player->GetOrder();
	GamePlayer* gamePlayer = new GamePlayer((Pick)player->GetPick(), 100, 0, player->GetUserId(), orderIndex);
	m_players[orderIndex] = gamePlayer;	
	m_session.insert(session);
}

void Game::ReleaseSession(GameServerSession* session)
{
	Player* player = session->GetPlayer();
	player->SetLocation(Location::WATING_ROOM);
	int orderIndex = player->GetOrder();
	delete m_players[orderIndex];
	m_players[orderIndex] = nullptr;
	m_session.erase(session);
}

void Game::Init(GameServerSession* session)
{
	{
		SendBuffer sendBuffer(nullptr, 4096);
		BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());
		PacketHeader* header = os.Reserve<PacketHeader>();

		int32 sessionCount = m_session.size();

		os << sessionCount;

		for (auto& session : m_session)
		{
			int32 pick = session->GetPlayer()->GetPick();
			int32 order = session->GetPlayer()->GetOrder();
			int32 usernameSize = session->GetPlayer()->GetUsernameSize();
			WCHAR* username = session->GetPlayer()->GetUsername();

			os << pick << order << usernameSize;
			os.Write(username, usernameSize);
		}

		sendBuffer.Close(os.WriteSize());
		header->size = os.WriteSize();
		header->id = PKT_C_GAMEINIT;

		BroadCast(&sendBuffer);
	}

	{
		for (auto& session : m_session)
		{
			SendBuffer sendBuffer(nullptr, 256);
			BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());
			PacketHeader* header = os.Reserve<PacketHeader>();
			int32 order = session->GetPlayer()->GetOrder();
			os << order;
			header->size = os.WriteSize();
			header->id = PKT_C_TURNINFO;
			sendBuffer.Close(os.WriteSize());
			session->Send(&sendBuffer);
		}
	}

	SendTurn();
}


void Game::BroadCast(SendBuffer* sendBuffer)
{
	for (auto& session : m_session) 
	{
		session->Send(sendBuffer);
	}
}

void Game::SendTurn()
{
	SendBuffer sendBuffer(nullptr, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());
	PacketHeader* header = os.Reserve<PacketHeader>();
	int32 turn = m_turnPlayer;
	os << turn;
	sendBuffer.Close(os.WriteSize());
	header->size = os.WriteSize();
	header->id = PKT_C_GAMETURNSEND;
	BroadCast(&sendBuffer);
}

void Game::NextTurn()
{
	if (m_players[m_turnPlayer] && m_players[m_turnPlayer]->GetDoubble())
	{
		if (m_players[m_turnPlayer]->ISISland())
		{
			m_players[m_turnPlayer]->SetDoubble(false);
		}
		else
		{
			m_players[m_turnPlayer]->SetDoubble(false);
			return;
		}
	}

	m_turnPlayer = (m_turnPlayer + 1) % 4;

	while (m_players[m_turnPlayer] == nullptr || (m_players[m_turnPlayer] != nullptr && m_players[m_turnPlayer]->ISISland()))
	{
		m_turnPlayer = (m_turnPlayer + 1) % 4;
	}
}

void Game::DiceDrop(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);

	PacketHeader header;
	int32 playerIndex;

	in >> header >> playerIndex;
	
	int32 dice1Number = (rand() % 6) + 1;
	int32 dice2Number = (rand() % 6) + 1;
	int32 diceSum = dice1Number + dice2Number;
	if (dice1Number == dice2Number)
	{
		m_players[playerIndex]->SetDoubble(true);
	}

	SendBuffer sendBuffer(session, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());
	PacketHeader* oheader = os.Reserve<PacketHeader>();


	os << dice1Number << dice2Number << diceSum << playerIndex;

	sendBuffer.Close(os.WriteSize());
	oheader->id = PKT_C_DICEDROP;
	oheader->size = os.WriteSize();
	BroadCast(&sendBuffer);
}

void Game::DiceDropEnd(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);

	PacketHeader header;
	int32 movePower;
	int32 playerIndex;

	in >> header >> movePower >> playerIndex;

	GamePlayer* movePlayer = m_players[playerIndex] ;
	Region destRegion = movePlayer->Move(movePower, m_map);
	bool salry = movePlayer->ISSalry();

	if (salry)
		movePlayer->AddMoney(10);

	int32 playerMoney = movePlayer->GetMoney();

	SendBuffer sendBuffer(session, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

	PacketHeader* oheader = os.Reserve<PacketHeader>();

	os << destRegion << playerIndex << salry << playerMoney;
	sendBuffer.Close(os.WriteSize());
	oheader->id = PKT_C_DICEDROPEND;
	oheader->size = os.WriteSize();

	BroadCast(&sendBuffer);
}

void Game::PlayerMoveEnd(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);

	PacketHeader header;
	int32 playerIndex;
	in >> header >> playerIndex;

	GamePlayer* player = m_players[playerIndex];
	int playerPos = player->GetPos();
	Region region = m_map->GetRegion(playerPos);

	// 통과지역
	if (m_map->CanPass(region._mapIndex))
	{
		NextTurn();
		SendTurn();
	}
	else if (region._mapIndex == 8) // 무인도
	{
		player->SetIsland();
		NextTurn();
		SendTurn();
	}
	else
	{
		// 1) 내땅인가?
		if (region._ownerPlayerIndx == playerIndex || region._ownerPlayerIndx == -1)
		{
			if (region._passCost <= player->GetMoney())
			{
				ProcessBuyRegionModalOpen(session, region);
			}
			else
			{
				NextTurn();
				SendTurn();
			}
		}
		else
		{
			// 내땅이 아니므로 통행료를 내라한다
			int cost = region._passCost;

			if (player->GetMoney() < cost) 		// 플레이어가 가진 금액이 통행료보다 비쌀 경우 
			{
				int regionSum = m_map->PlayerRegionSum(playerIndex);
				int money = regionSum + player->GetMoney();

				m_map->DestoryRegionByPlayerIdx(playerIndex);

				player->SubMoney(cost);
				GamePlayer* other = m_players[region._ownerPlayerIndx];
				other->AddMoney(money);

				ProcessPlayerDead(session,playerIndex, region._ownerPlayerIndx,money);
			}
			else 	// 플레이어가 가진 금액이 통행료보다 많거나 같을 경우
			{
				// 통행료를 걷는다
				player->SubMoney(cost);
				GamePlayer* other = m_players[region._ownerPlayerIndx];
				other->AddMoney(cost);
				int passAfterMoney = player->GetMoney();
				int passAfterMoneyOther = other->GetMoney();

				ProcessPlayerPayCost(session,playerIndex, passAfterMoney, region._ownerPlayerIndx, passAfterMoneyOther);

				if (passAfterMoney >= cost)
				{
					ProcessBuyOtherRegionModalOpen(session, region);
				}
				else
				{
					NextTurn();
					SendTurn();
				}
			}
		}
	}
}

void Game::ProcessBuyRegionModalOpen(GameServerSession* session, Region region)
{
	SendBuffer sendBuffer(session, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

	PacketHeader* header = os.Reserve<PacketHeader>();
	os << region;
	header->id = PKT_C_BUYREGION;
	header->size = os.WriteSize();
	sendBuffer.Close(os.WriteSize());
	session->Send(&sendBuffer);
}

void Game::ProcessBuyOtherRegionModalOpen(GameServerSession* session, Region region)
{
	SendBuffer sendBuffer(session, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

	PacketHeader* header = os.Reserve<PacketHeader>();
	os << region;
	header->id = PKT_C_BUYOTHERREGION;
	header->size = os.WriteSize();
	sendBuffer.Close(os.WriteSize());
	session->Send(&sendBuffer);
}

void Game::ProcessPlayerDead(GameServerSession* session, int32 playerIndex, int32 owerIndex, int32 money)
{
	SendBuffer sendBuffer(session, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

	PacketHeader* header = os.Reserve<PacketHeader>();
	os << playerIndex << owerIndex << money;

	header->id = PKT_C_PLAYERDEAD;
	header->size = os.WriteSize();

	sendBuffer.Close(os.WriteSize());
	BroadCast(&sendBuffer);
}

void Game::ProcessPlayerPayCost(GameServerSession* session, int32 playerIndex, int32 passAfterMoney, int32 ownerIndex, int32 passAfterMoneyOther)
{
	SendBuffer sendBuffer(session, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

	PacketHeader* header = os.Reserve<PacketHeader>();
	os << playerIndex << passAfterMoney << ownerIndex << passAfterMoneyOther;

	header->id = PKT_C_PASSCOST;
	header->size = os.WriteSize();

	sendBuffer.Close(os.WriteSize());
	BroadCast(&sendBuffer);
}

void Game::BuyRegion(GameServerSession* session, BYTE* buffer, int32 len)
{

	BufferInputStream in(buffer, len);

	PacketHeader header;
	int32 playerIndex;
	in >> header >> playerIndex;

	GamePlayer* player = m_players[playerIndex];
	Region r = m_map->GetRegion(player->GetPos());

	m_map->BuyRegion(player, playerIndex);
	Region region = m_map->GetRegion(player->GetPos());

	int playerMoney = player->GetMoney();

	SendBuffer sendBuffer(session, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

	PacketHeader* oheader = os.Reserve<PacketHeader>();
	os << playerIndex << region << playerMoney;

	oheader->id = PKT_C_BUYREGIONRESPONSE;
	oheader->size = os.WriteSize();
	sendBuffer.Close(os.WriteSize());

	BroadCast(&sendBuffer);
	m_map->MapLevelUp(player);
}

void Game::BuyOtherRegion(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);

	PacketHeader header;
	int32 playerIndex;
	in >> header >> playerIndex;

	GamePlayer* player = m_players[playerIndex];
	Region r = m_map->GetRegion(player->GetPos());
	GamePlayer* other = m_players[r._ownerPlayerIndx];
	int otherIdx = r._ownerPlayerIndx;

	m_map->BuyOtherRegion(player, other, playerIndex);
	Region region = m_map->GetRegion(player->GetPos());

	int playerMoney = player->GetMoney();
	int otherMoney = other->GetMoney();

	SendBuffer sendBuffer(session, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

	PacketHeader* oheader = os.Reserve<PacketHeader>();
	os << playerIndex << region << playerMoney << otherMoney << otherIdx;

	oheader->id = PKT_C_BUYREGIONOTHERRESPONSE;
	oheader->size = os.WriteSize();
	sendBuffer.Close(os.WriteSize());

	BroadCast(&sendBuffer);
}

bool Game::PlayerDead(GameServerSession* session, BYTE* buffer, int32 len)
{
	BufferInputStream in(buffer, len);
	PacketHeader header;
	int32 playerIndex;
	in >> header >> playerIndex;

	delete m_players[playerIndex];
	m_players[playerIndex] = nullptr;
	m_map->DestoryRegionByPlayerIdx(playerIndex);

	if (PlayerCount() == 1)
	{
		int player;
		for (player = 0; player < 4; player++)
		{
			if (m_players[player] != nullptr)
				break;
		}

		SendBuffer sendBuffer(session, 256);
		BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

		PacketHeader* oheader = os.Reserve<PacketHeader>();
		os << player;

		oheader->id = PKT_C_GAMEEND;
		oheader->size = os.WriteSize();
		sendBuffer.Close(os.WriteSize());

		BroadCast(&sendBuffer);

		for (auto& s : m_session)
		{
			if (s)
			{
				s->GetPlayer()->SetLocation(Location::WATING_ROOM);
				s->GetPlayer()->SetReady(false);
			}
		}
		return true;
	}
	else
	{
		NextTurn();
		SendTurn();
		return false;
	}
}

bool Game::PlayerDead(int32 playerIndex)
{
	delete m_players[playerIndex];
	m_players[playerIndex] = nullptr;
	m_map->DestoryRegionByPlayerIdx(playerIndex);

	if (PlayerCount() == 1)
	{
		int player;
		for (player = 0; player < 4; player++)
		{
			if (m_players[player] != nullptr)
				break;
		}

		SendBuffer sendBuffer(nullptr, 256);
		BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());

		PacketHeader* oheader = os.Reserve<PacketHeader>();
		os << player;

		oheader->id = PKT_C_GAMEEND;
		oheader->size = os.WriteSize();
		sendBuffer.Close(os.WriteSize());

		BroadCast(&sendBuffer);

		for (auto& s : m_session)
		{
			if (s)
			{
				s->GetPlayer()->SetLocation(Location::WATING_ROOM);
				s->GetPlayer()->SetReady(false);
			}
		}
		return true;
	}
	else
	{
		NextTurn();
		SendTurn();
		return false;
	}
}

int Game::PlayerCount()
{
	int ret = 0;
	for (int i = 0; i < 4; i++)
	{
		if (m_players[i] != nullptr)
			ret++;
	}
	return ret;
}
bool Game::ISEnd()
{
	int playerCount = PlayerCount();
	return playerCount == 1;
}
void Game::PlayerDisconnect(int32 playerIndex)
{
	delete m_players[playerIndex];
	m_players[playerIndex] = nullptr;
	m_map->DestoryRegionByPlayerIdx(playerIndex);

	SendBuffer sendBuffer(nullptr, 256);
	BufferOutputStream os(sendBuffer.Buffer(), sendBuffer.AllocSize());
	PacketHeader* oheader = os.Reserve<PacketHeader>();
	os << playerIndex;

	oheader->id = PKT_C_DISCONNECT;
	oheader->size = os.WriteSize();
	sendBuffer.Close(os.WriteSize());

	BroadCast(&sendBuffer);

	if (m_turnPlayer == playerIndex)
	{
		NextTurn();
		SendTurn();
	}
}

