#pragma once
class GamePlayer
{
private:
	Pick m_pick;
	int	 m_money = 100;
	int	 m_pos = 0;
	bool m_doubble = false;
	bool m_salry = false;
	int  m_islandCount = 0;
	int  m_userId = 0;
	int  m_order = 0;
public:
	GamePlayer(Pick _pick, int _money, int _pos, int _userId, int _order);
	~GamePlayer();

	int	   GetPos() { return m_pos; }
	int    GetMoney() { return m_money; }

	void   AddMoney(int _money);
	void   SubMoney(int _money);
	Region Move(int _power, class Map* _map);

	void SetDoubble(bool _double) { m_doubble = _double; }
	bool GetDoubble() { return m_doubble; }
	bool ISSalry();
	bool ISISland();
	void SetIsland();
	int  GetUserId() { return m_userId; }
	int32 GetOrder() { return m_order; }
};

