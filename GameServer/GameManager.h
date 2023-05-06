#pragma once
class Game;
class GameManager
{
private:
	std::map<int32, Game*> m_game;

public:
	void				  Add(int32 roomSq, Game* room);
	void				  Release(int32 roomSq);
	Game*				  GetGame(int32 roomSq);
	std::map<int, Game*>& GetGameData() { return m_game; }
	int32				  GetGameCount() { return m_game.size(); }
};

extern GameManager GGameManager;

