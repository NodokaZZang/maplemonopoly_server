#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\SelectServerCore.lib")
#else
#pragma comment(lib, "Release\\SelectServerCore.lib")
#endif

enum Location : int
{
	LOBBY_ROOM = 0,
	WATING_ROOM = 1,
	GAME_ROOM = 2
};

enum Pick : int
{
	HORN_MURSHROOM = 0,
	ORANGE_MURSHROOM = 1,
	PIG = 2,
	NOUSER = 3
};

enum : int
{
	PKT_S_LOGIN = 0,
	PKT_S_LOBBYASYNC = 1,
	PKT_C_LOBBYASYNC = 2,
	PKT_S_LOBBYCHAT = 3,
	PKT_C_LOBBYCHAT = 4,
	PKT_S_CREATEROOM = 5,
	PKT_S_ENTERROOM = 6,
	PKT_C_WROOM_ENTEROTHERUSER = 7,
	PKT_C_WROOM_ENTERUSER = 8,
	PKT_S_WROOMCHAT = 9,
	PKT_C_WROOMCHAT = 10,
	PKT_S_EXITWROOM = 11,
	PKT_S_PICKCHANGE = 12,
	PKT_C_WROOM_ASYNCUSERLIST = 13,
	PKT_C_PICKCHANGE = 14,
	PKT_S_READY = 15,
	PKT_C_READY = 16,
	PKT_C_ENTERGAME = 17,
	PKT_C_GAMEINIT = 18,
	PKT_C_GAMETURNSEND = 19,
	PKT_C_TURNINFO = 20,
	PKT_S_DICEDROP = 21,
	PKT_C_DICEDROP = 22,
	PKT_S_DICEDROPEND = 23,
	PKT_C_DICEDROPEND = 24,
	PKT_S_PLAYERMOVEEND = 25,
	PKT_C_PLAYERMOVEEND = 26,
	PKT_C_BUYREGION = 27,
	PKT_C_PLAYERDEAD = 28,
	PKT_C_PASSCOST = 29,
	PKT_C_BUYOTHERREGION = 30,
	PKT_S_PASSTURN = 31,
	PKT_S_BUYOTHERREGION = 32,
	PKT_S_BUYREGION = 33,
	PKT_C_BUYREGIONRESPONSE = 34,
	PKT_C_BUYREGIONOTHERRESPONSE = 35,
	PKT_S_PLAYERDEAD = 36,
	PKT_C_GAMEEND = 37,
	PKT_C_DISCONNECT = 38,
};

enum RoomStatus : int 
{
	HOLD = 0,
	GAME = 1
};

struct Region
{
	int _ownerPlayerIndx = -1;
	int _passCost = 0;
	int _mapIndex = 0;
	int _level = 0;
	int _oriCost = 0;
};

#include "CorePch.h"