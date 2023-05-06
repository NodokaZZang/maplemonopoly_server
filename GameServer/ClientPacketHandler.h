#pragma once

class GameServerSession;
class ClientPacketHandler
{
public:
	void HandlePacket(GameServerSession* session, BYTE* buffer, int32 len);

private:
	void HandlePacket_LOGIN(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_LOBBYASYNC(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_LOBBYCHAT(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_CREATEROOM(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_ENTERROOM(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_WROOMCHAT(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_WROOMEXIT(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_WROOMPICKCHANGE(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_WROOMREADY(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_DICEDROP(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_PLAYERMOVEEND(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_PASSTURN(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_BUYREGION(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_BUYOTHERREGION(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_DICEDROPEND(GameServerSession* session, BYTE* buffer, int32 len);
	void HandlePacket_PLAYERDEAD(GameServerSession* session, BYTE* buffer, int32 len);
};

extern ClientPacketHandler GClinetPacketHandler;