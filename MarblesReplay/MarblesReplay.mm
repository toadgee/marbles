//
//	main.m
//	MarblesReplay
//
//	Created by Todd on 2/1/13.
//	Copyright (c) 2013 toadgee.com. All rights reserved.
//

#include "Game.h"
#include "GameLog.h"
#include "Player.h"
#include "MoveGenerator.h"
#include "MoveRanker.h"

static void PrintUsage();
static void ReadLine();

void PlayerWillPlayMoveInGame(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move);
void PlayerPlayedMoveInGame(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move);
void HandEndedInGame(void* context, TGMGame* game);
void TurnStartedInGame(void* context, TGMGame* game, TGMPlayer* player);
void HandStartedInGame(void* context, TGMGame* game);
void GameStarted(void* context, TGMGame* game);
void ReplayFinished(void* context, TGMGame* game, TGMGameLog* gameLog);

void ReadLine()
{
	size_t length = 0;
	fgetln(stdin, &length);
}

int main(int argc, const char * argv[])
{
	@autoreleasepool
	{
		NSString* filename = nil;
		for (int arg = 1; arg < argc; arg++)
		{
			NSString* argStr = [[NSString alloc] initWithCString:argv[arg] encoding:NSUTF8StringEncoding];
			NSString* lowerArg = [argStr lowercaseString];
			if ([lowerArg isEqualToString:@"--filename"])
			{
				arg++;
				if (arg >= argc)
				{
					printf("Must supply filename.\n");
					exit(1);
				}
				
				filename = [[NSString alloc] initWithCString:argv[arg] encoding:NSUTF8StringEncoding];
				if ([filename hasPrefix:@"--"])
				{
					printf("Must supply valid filename.\n");
					exit(1);
				}
			}
			else if ([lowerArg isEqualToString:@"--?"]
				|| [lowerArg isEqualToString:@"-?"]
				|| [lowerArg isEqualToString:@"/?"]
				|| [lowerArg isEqualToString:@"--help"]
				|| [lowerArg isEqualToString:@"-help"]
				|| [lowerArg isEqualToString:@"/help"])
			{
				PrintUsage();
				exit(0);
			}
			else
			{
				printf("Unknown argument : %s\n", argv[arg]);
				PrintUsage();
				exit(1);
			}
		}
		
		if ([filename length] == 0)
		{
			printf("--filename required. Use --help for details.\n");
			PrintUsage();
			exit(1);
		}
		
		filename = [filename stringByExpandingTildeInPath];
		NSData* gameData = [NSData dataWithContentsOfFile:filename];
		if (!gameData)
		{
			printf("File '%s' could not be loaded\n", [filename UTF8String]);
			exit(1);
		}
		
		TGMGameLog* gameLog = CreateGameLogFromData(gameData);
		if (!gameLog)
		{
			printf("File '%s' is not a valid game log\n", [filename UTF8String]);
			exit(1);
		}
		
		printf("Replaying gamelog: \n%s\n", [GameLogDescription(gameLog) UTF8String]);
		TGMGame* replayGame = CreateGame(gameLog, nullptr);
		GameSetOnPlayerDidPlay(replayGame, &PlayerPlayedMoveInGame);
		GameSetOnPlayerWillPlay(replayGame, &PlayerWillPlayMoveInGame);
		GameSetOnEndedHand(replayGame, &HandEndedInGame);
		GameSetOnStartedTurn(replayGame, &TurnStartedInGame);
		GameSetOnStartedHand(replayGame, &HandStartedInGame);
		GameSetOnDidStart(replayGame, &GameStarted);
		GameSetOnDidFinishReplay(replayGame, &ReplayFinished);
		
		GameStartNew(replayGame);
	}
	
	return 0;
}

void PrintUsage()
{
	printf("MarblesReplay --filename [foo.marbles]\n");
}


void GameStarted(void* context, TGMGame* game)
{
	printf("Game started\n");
}


void ReplayFinished(void* context, TGMGame* game, TGMGameLog* gameLog)
{
	printf("Game finished replay! Press enter to exit.\n");
	ReadLine();
	exit(0);
}

void HandStartedInGame(void* context, TGMGame* game)
{
	printf("Hand started\n");
}

void HandEndedInGame(void* context, TGMGame* game)
{
	printf("Hand ended\n");
}

void TurnStartedInGame(void* context, TGMGame* game, TGMPlayer* player)
{
	printf("Turn #%d started for %s\n", GameGetTurn(game), [PlayerDescription(player) UTF8String]);
}

void PlayerWillPlayMoveInGame(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move)
{
	printf("#%d    %s ===> %s\n", GameGetTurn(game), [PlayerDescription(player) UTF8String], [MoveDescription(move) UTF8String]);
	
	TGMMoveList* allMoves = MovesForPlayerSimple(player, game, NULL);
	printf("All Moves : \n");
	__block BOOL foundGenMove = NO;
	
	__block TGMMoveList *allPossibleOpponentMoves = NULL;
	MoveListIterateWithBlock(allMoves, ^(int i, TGMMove *genMove)
	{
		BOOL isGenMove = AreMovesEqual(genMove, move);
		if (!foundGenMove && isGenMove)
			foundGenMove = YES;
	
		if (PlayerGetStrategy(player) != Strategy_Human)
		{
			CalculateWeightOfMoveInGame(genMove, game, PlayerGetColor(player), PlayerGetStrategy(player), &allPossibleOpponentMoves); // calculate so we output the weight
		}
		printf("\t%s%s\n", (isGenMove ? "***" : "   "), [MoveDescription(genMove) UTF8String]);
	});
	
	ReleaseMoveList(allPossibleOpponentMoves);
	
	if (PlayerGetStrategy(player) != Strategy_Human)
	{
		TGMMove* ourBestMove = BestMoveFromMoves(allMoves, game, PlayerGetColor(player), PlayerGetStrategy(player));
		if (!AreMovesEquivalent(ourBestMove, move))
		{
			printf("%s\n", [ColoredString(Player_Yellow, "### NOTE : Different move than what we would have picked") UTF8String]);
			printf("%s\n", [MoveDescription(ourBestMove) UTF8String]);
		}
	}
	
	printf("%s", [CardListDescription(PlayerGetHand(player), YES) UTF8String]);
}

void PlayerPlayedMoveInGame(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move)
{
	printf("%s\n", [BoardDescription(GameGetBoard(game)) UTF8String]);
	ReadLine();
}

