//
//	AITest.m
//	Marbles
//
//	Created by todd harris on 11/27/11.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#include "precomp.h"
#include "AITest.h"
#include "DebugAssert.h"
#include "Descriptions.h"
#include "Game.h"
#include "ComputerPlayer.h"
#include "Logging.h"
#include "Random.h"

void AITest_GameStarted(void* context, TGMGame* game);
void AITest_PlayerWillPlay(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move);
void AITest_PlayerDidPlay(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move);
void AITest_PlayerDiscarded(void* context, TGMGame* game, TGMPlayer* player, TGMCard* card);
void AITest_HandStarted(void* context, TGMGame* game);
void AITest_HandEnded(void* context, TGMGame* game);
void AITest_TurnStarted(void* context, TGMGame* game, TGMPlayer* player);
void AITest_TurnEnded(void* context, TGMGame* game, TGMPlayer* player);
void AITest_GameEnded(void* context, TGMGame* game, bool team1Won);
void AITest_PlayerKilledPlayer(void* context, TGMGame* game, TGMPlayer* player, TGMPlayer* deadPlayer);

/*
@interface AITest (PrivateMethods)
-(void)handEndedInGame:(TGMGame*)game;
-(void)turnEndedInGame:(TGMGame*)game forPlayer:(TGMPlayer*)player;
-(void)gameFinished:(TGMGame*)game team1Won:(BOOL)team1Won;
-(void)player:(TGMPlayer*)player1 killedPlayer:(TGMPlayer*)player2;
@end
*/
AITest::AITest() :
	AITest(Strategy::Aggressive, Strategy::Passive, 0)
{
}

AITest::AITest(Strategy team1, Strategy team2, unsigned seed)
	: _seed(seed)
{

	_gameTimer = CreateHiResTimer(true, false);
	
	// sets up game (which can be reused)
	_game = CreateGame(NULL, NULL);
	GameSetCallbackContext(_game, (void*)this);
		
#ifdef DEBUG_LOGGING_ON
	GameSetOnDidStart(_game, &AITest_GameStarted);
	GameSetOnPlayerWillPlay(_game, &AITest_PlayerWillPlay);
	GameSetOnPlayerDidPlay(_game, &AITest_PlayerDidPlay);
	GameSetOnPlayerDiscarded(_game, &AITest_PlayerDiscarded);
	GameSetOnStartedHand(_game, &AITest_HandStarted);
	GameSetOnStartedTurn(_game, &AITest_TurnStarted);
#endif
		
	GameSetOnEndedHand(_game, &AITest_HandEnded);
	GameSetOnEndedTurn(_game, &AITest_TurnEnded);
	GameSetOnDidFinish(_game, &AITest_GameEnded);
	GameSetOnPlayerDidKillPlayer(_game, &AITest_PlayerKilledPlayer);
		
	TGMPlayer* player;
	// team 1
	player = CreateComputerPlayer("Aubrey", team1, Player_Red);
	GameAddPlayer(_game, player);
	ReleasePlayer(player);
	
	player = CreateComputerPlayer("Katie", team1, Player_White);
	GameAddPlayer(_game, player);
	ReleasePlayer(player);
	
	player = CreateComputerPlayer("Shauna", team1, Player_Blue);
	GameAddPlayer(_game, player);
	ReleasePlayer(player);
	
	// team 2
	player = CreateComputerPlayer("Jason", team2, Player_Yellow);
	GameAddPlayer(_game, player);
	ReleasePlayer(player);
		
	player = CreateComputerPlayer("Ryan", team2, Player_Green);
	GameAddPlayer(_game, player);
	ReleasePlayer(player);
	
	player = CreateComputerPlayer("Todd", team2, Player_Black);
	GameAddPlayer(_game, player);
	ReleasePlayer(player);
}

AITest::~AITest()
{
	DestroyHiResTimer(_gameTimer);
	ReleaseGame(_game);
}

void AITest::Run(uint64_t test)
{
	HiResTimerStart(_gameTimer);
	GameStartNewWithSeed(_game, _seed);
	uint64_t elapsed = HiResTimerStop(_gameTimer);
	
	if (_statusUpdates != 0 && test % _statusUpdates == 0)
	{
		printf("Game %llu took %llu milliseconds\n", test, HiResTimerElapsedToMilliseconds(_gameTimer, elapsed));
	}
	
	return;
}

void AITest::HandEnded(TGMGame * game)
{
	(void)game;
	dassert(game == _game);
	_handsTotal++;
}

void AITest::TurnEnded(TGMGame* game, TGMPlayer * /*player*/)
{
	(void)game;
	dassert(game == _game);
	_turnsTotal++;
	
#ifdef DEBUG_LOGGING_ON
	TGMLogGameMessage(@"</turn %d> - Current player is : %@", GameGetTurn(game), PlayerDescription(player));
#else
	//TGMLogGameMessage(@"</turn %lld>", game.turn);
#endif
}

void AITest::GameEnded(TGMGame* game, bool team1Won)
{
	dassert(game == _game);
	if (team1Won)
	{
		_team1WonGames++;
	}
	else
	{
		_team2WonGames++;
	}
	
	// determine how the dealing color did
	bool team1Dealt = IsPlayerColorTeam1(GameLogDealingPlayer(GameGetGameLog(game)));
	bool dealingTeamWon = (team1Won == team1Dealt);
	if (dealingTeamWon)
	{
		_dealingTeamWonGames++;
	}
	
	
#ifdef DEBUG_LOGGING_ON
	TGMLogGameMessage(@"WINNER: %@", team1Won ? @"Team 1" : @"Team 2");
#endif
}

void AITest::PlayerKilledPlayer(TGMGame* /*game*/, TGMPlayer *player, TGMPlayer *deadPlayer)
{
	_totalKills++;
	if (PlayerIsTeam1(player) && PlayerIsTeam1(deadPlayer))
	{
		_team1KilledTeam1++;
	}
	else if (!PlayerIsTeam1(player) && !PlayerIsTeam1(deadPlayer))
	{
		_team2KilledTeam2++;
	}
	else if (PlayerIsTeam1(player) && !PlayerIsTeam1(deadPlayer))
	{
		_team1KilledTeam2++;
	}
	else
	{
		_team2KilledTeam1++;
	}
	
#ifdef DEBUG_LOGGING_ON
	TGMLogGameMessage(@"%@ killed by %@", player2, player1);
#endif
}

void AITest_GameStarted(void* /*context*/, TGMGame* /*game*/)
{
	printf("Starting game...\n");
}

void AITest_PlayerWillPlay(void* /*context*/, TGMGame* game, TGMPlayer* player, TGMMove* move)
{
	printf("%s\n", BoardDescription(GameGetBoard(game)).c_str());
	printf("%s move is : %s\n", PlayerDescription(player).c_str(), MoveDescription(move).c_str());
}

void AITest_PlayerDidPlay(void* /*context*/, TGMGame* game, TGMPlayer* /*player*/, TGMMove* /*move*/)
{
	printf("%s\n", BoardDescription(GameGetBoard(game)).c_str());
}

void AITest_PlayerDiscarded(void* /*context*/, TGMGame* /*game*/, TGMPlayer* player, TGMCard* card)
{
	printf("%s discarded %s\n", PlayerDescription(player).c_str(), CardDescription(card, false).c_str());
}

void AITest_HandStarted(void* /*context*/, TGMGame* /*game*/)
{
	printf("Starting hand...\n");
}

void AITest_HandEnded(void* context, TGMGame* game)
{
	((AITest*)context)->HandEnded(game);
}

void AITest_TurnStarted(void* /*context*/, TGMGame* game, TGMPlayer* player)
{
	printf("<turn %d> - Current player is : %s\n", GameGetTurn(game), PlayerDescription(player).c_str());
}

void AITest_TurnEnded(void* context, TGMGame* game, TGMPlayer* player)
{
	((AITest*)context)->TurnEnded(game, player);
}

void AITest_GameEnded(void* context, TGMGame* game, bool team1Won)
{
	((AITest*)context)->GameEnded(game, team1Won);
}

void AITest_PlayerKilledPlayer(void* context, TGMGame* game, TGMPlayer* player, TGMPlayer* deadPlayer)
{
	((AITest*)context)->PlayerKilledPlayer(game, player, deadPlayer);
}

