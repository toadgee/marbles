//
//	main.m
//	MarblesReplay
//
//	Created by Todd on 2/1/13.
//	Copyright (c) 2013 toadgee.com. All rights reserved.
//
#include "precomp.h"
#include "Game.h"
#include "GameLog.h"
#include "DataExtensions.h"
#include "Descriptions.h"
#include "Player.h"
#include "MoveGenerator.h"
#include "MoveRanker.h"

void ReplayPlayerWillPlayMoveInGame(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move) noexcept;
void ReplayPlayerPlayedMoveInGame(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move) noexcept;
void ReplayHandEndedInGame(void* context, TGMGame* game) noexcept;
void ReplayTurnStartedInGame(void* context, TGMGame* game, TGMPlayer* player) noexcept;
void ReplayHandStartedInGame(void* context, TGMGame* game) noexcept;
void ReplayGameStarted(void* context, TGMGame* game) noexcept;
[[noreturn]] void ReplayFinished(void* context, TGMGame* game, TGMGameLog* gameLog) noexcept;

void ReadLine() noexcept
{
	//size_t length = 0;
	//fgetln(stdin, &length);
}

void ReplayGame(std::string filename) noexcept
{
	//NSData* gameData = [NSData dataWithContentsOfFile:filename];
#if 0
	if (!gameData)
	{
		printf("File '%s' could not be loaded\n", [filename UTF8String]);
		exit(1);
	}
	
	TGMGameLog* gameLog = CreateGameLogFromData(gameData);
	if (!gameLog)
	{
		printf("File '%s' is not a valid game log\n", filename.c_str());
		exit(1);
	}
#endif
	TGMGameLog* gameLog = nullptr;
	printf("Replaying gamelog: \n%s\n", GameLogDescription(gameLog).c_str());
	TGMGame* replayGame = CreateGame(gameLog, nullptr);
	GameSetOnPlayerDidPlay(replayGame, &ReplayPlayerPlayedMoveInGame);
	GameSetOnPlayerWillPlay(replayGame, &ReplayPlayerWillPlayMoveInGame);
	GameSetOnEndedHand(replayGame, &ReplayHandEndedInGame);
	GameSetOnStartedTurn(replayGame, &ReplayTurnStartedInGame);
	GameSetOnStartedHand(replayGame, &ReplayHandStartedInGame);
	GameSetOnDidStart(replayGame, &ReplayGameStarted);
	GameSetOnDidFinishReplay(replayGame, &ReplayFinished);
	
	GameStartNew(replayGame);
	
	return;
}

void ReplayGameStarted(void* context, TGMGame* game) noexcept
{
	printf("Game started\n");
}

void ReplayFinished(void* context, TGMGame* game, TGMGameLog* gameLog) noexcept
{
	printf("Game finished replay! Press enter to exit.\n");
	ReadLine();
	exit(0);
}

void ReplayHandStartedInGame(void* context, TGMGame* game) noexcept
{
	printf("Hand started\n");
}

void ReplayHandEndedInGame(void* context, TGMGame* game) noexcept
{
	printf("Hand ended\n");
}

void ReplayTurnStartedInGame(void* context, TGMGame* game, TGMPlayer* player) noexcept
{
	printf("Turn #%d started for %s\n", GameGetTurn(game), PlayerDescription(player).c_str());
}

void ReplayPlayerWillPlayMoveInGame(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move) noexcept
{
	printf("#%d    %s ===> %s\n", GameGetTurn(game), PlayerDescription(player).c_str(), MoveDescription(move).c_str());
	
	TGMMoveList* allMoves = MovesForPlayerSimple(player, game, nullptr);
	printf("All Moves : \n");
	bool foundGenMove = false;
	TGMMoveList *allPossibleOpponentMoves = nullptr;
	TGMMove *genMove = allMoves->first;
	while (genMove != nullptr)
	{
		const bool isGenMove = AreMovesEqual(genMove, move);
		if (!foundGenMove && isGenMove)
			foundGenMove = true;
	
		if (PlayerGetStrategy(player) != Strategy::Human)
		{
			CalculateWeightOfMoveInGame(genMove, game, PlayerGetColor(player), PlayerGetStrategy(player), &allPossibleOpponentMoves); // calculate so we output the weight
		}
		printf("\t%s%s\n", (isGenMove ? "***" : "   "), MoveDescription(genMove).c_str());
		genMove = genMove->nextMove;
	}
	
	ReleaseMoveList(allPossibleOpponentMoves);
	
	if (PlayerGetStrategy(player) != Strategy::Human)
	{
		TGMMove* ourBestMove = BestMoveFromMoves(allMoves, game, PlayerGetColor(player), PlayerGetStrategy(player));
		if (!AreMovesEquivalent(ourBestMove, move))
		{
			printf("%s\n", ColoredString(PlayerColor::Yellow, "### NOTE : Different move than what we would have picked").c_str());
			printf("%s\n", MoveDescription(ourBestMove).c_str());
		}
	}
	
	printf("%s", CardListDescription(PlayerGetHand(player), true).c_str());
}

void ReplayPlayerPlayedMoveInGame(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move) noexcept
{
	printf("%s\n", BoardDescription(GameGetBoard(game)).c_str());
	ReadLine();
}
