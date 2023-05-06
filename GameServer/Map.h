#pragma once
class GamePlayer;
class Map
{
private:
	int					m_mapCount = 32;
	std::vector<Region> m_map;
public:
	Map();
	~Map();

	Region GetRegion(int _pos) { return m_map[_pos]; };
	void   BuyRegion(GamePlayer* _buyer, int _playerIdx);
	void   BuyRegion(GamePlayer* _buyer, GamePlayer* _seller, int _playerIdx);
	void   DestoryRegionByPlayerIdx(int _playerIdx);
	void   BuyOtherRegion(GamePlayer* _buyer, int _playerIdx);
	void   MapLevelUp(GamePlayer* _buyer);
	void   BuyOtherRegion(GamePlayer* _buyer, GamePlayer* _seller, int _playerIdx);
	bool   CanPass(int _pos);
	int	   PlayerRegionSum(int _playerIndex);
};

