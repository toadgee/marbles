//
//	AITest.h
//	Marbles
//
//	Created by todd harris on 11/27/11.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#include "Game.h"
#include "HiResTimer.h"

class AITest
{
public:

	TGHiResTimer* GetGameTimer() { return _gameTimer; }
	uint64_t GetHandsTotal() { return _handsTotal;  }
	uint64_t GetTurnsTotal() { return _turnsTotal;  }

	uint64_t GetTotalKills() { return _totalKills; }
	uint64_t GetTeam1KilledTeam1() { return _team1KilledTeam1; }
	uint64_t GetTeam1KilledTeam2() { return _team1KilledTeam2; }
	uint64_t GetTeam2KilledTeam1() { return _team2KilledTeam1; }
	uint64_t GetTeam2KilledTeam2() { return _team2KilledTeam2; }

	uint64_t GetTeam1WonGames() { return _team1WonGames; }
	uint64_t GetTeam2WonGames() { return _team2WonGames; }
	uint64_t GetDealingTeamWonGames() { return _dealingTeamWonGames;  }

	uint64_t GetStatusUpdates() { return _statusUpdates; }
	void SetStatusUpdates(uint64_t statusUpdates) { _statusUpdates = statusUpdates; }
	unsigned GetSeed() { return _seed; }

	AITest();
	AITest(Strategy team1, Strategy team2, unsigned seed);
	~AITest();
	void Run(uint64_t test);

	// TODO : make these functions so i don't need a weird public thing
	void HandEnded(TGMGame *game);
	void TurnEnded(TGMGame* game, TGMPlayer* player);
	void PlayerKilledPlayer(TGMGame* game, TGMPlayer* player, TGMPlayer* playerWhoDied);
	void GameEnded(TGMGame* game, bool team1Won);

private:
	unsigned _seed;
	TGHiResTimer* _gameTimer{};
	uint64_t _handsTotal{};
	uint64_t _turnsTotal{};

	uint64_t _statusUpdates{};

	uint64_t _totalKills{};
	uint64_t _team1KilledTeam1{};
	uint64_t _team1KilledTeam2{};
	uint64_t _team2KilledTeam1{};
	uint64_t _team2KilledTeam2{};

	uint64_t _team1WonGames{};
	uint64_t _team2WonGames{};
	uint64_t _dealingTeamWonGames{};

	TGMGame* _game{};
};
