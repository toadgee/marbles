#include "precomp.h"
#include "ComputerPlayer.h"
#include "ConsolePlayer.h"
#include "ConsoleUtilities.h"
#include "DebugAssert.h"
#include "Descriptions.h"
#include "Player.h"
#include "Game.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "MoveList.h"
#include "MoveRanker.h"
#include "Card.h"
#include "MarblesMem.h"

void DisplayHand(TGMCardList* hand, bool isSelf);
bool DoesPlayerHaveAnyNonDiscardMoves(TGMPlayer* player, TGMCard* card, TGMMoveList* moves, TGMGame* game);
void PlayInGame(TGMPlayer* player);
void PlayedMove(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move);
void HandStarted(void* context, TGMGame* game);
void TurnStarted(void* context, TGMGame* game, TGMPlayer* player);
void TurnEnded(void* context, TGMGame* game, TGMPlayer* player);
void GameStarted(void* context, TGMGame* game);
void GameFinished(void* context, TGMGame* game, bool team1Won);
void PlayerKilledPlayer(void* context, TGMGame* game, TGMPlayer* player, TGMPlayer* deadPlayer);

void SimulateGame(TGMGame* game, bool* playerTeamWon, bool output)
{
	TGMGame* copyGame = CopyGame(game);
	RestoreRandom(GameGetRandomNumberGenerator(copyGame));
	
	bool humanIsTeam1 = false;
	for (int i = 0; i < kPlayers; ++i)
	{
		TGMPlayer* p = GameGetPlayerAtIndex(game, i);
		
		PlayerColor pc = PlayerGetColor(p);
		MarbleStrategy strategy = PlayerGetStrategy(p);
		const char* name = PlayerGetName(p);
		
		if (strategy == Strategy_Human)
		{
			humanIsTeam1 = PlayerIsTeam1(p);
			
			// mark the strategy as the other players on the team.
			TGMTeammates* teammates = GameTeammatesForPlayer(game, pc);
			TGMPlayer* p2 = TeammatesPlayer(teammates, 0);
			strategy = PlayerGetStrategy(p2);
		}
		
		TGMPlayer* player = CreateComputerPlayer(name, strategy, pc);
		GameAddPlayer(copyGame, player);
		
		PlayerCopyData(p, player);
	}
	
	GameContinue(copyGame);

	uint8_t team1Score = GameGetTeam1Score(copyGame);
	uint8_t team2Score = GameGetTeam2Score(copyGame);
	bool team1Won = (team1Score > team2Score);
	
	if (output)
	{
		if (team1Won == humanIsTeam1)
		{
			printf("Computer would win this game if human wasn't playing (Score=%u to %u)\n", team1Score, team2Score);
		}
		else
		{
			printf("Computer would lose this game if human wasn't playing (Score=%u to %u)\n", team1Score, team2Score);
		}
	}
	
	RestoreRandom(GameGetRandomNumberGenerator(game));
	ReleaseGame(copyGame);
	
	if (playerTeamWon != nullptr)
	{
		*playerTeamWon = (team1Won == humanIsTeam1);
	}
}

static bool s_outputHowTheComputerWouldDo = false;
TGMPlayer* CreateConsolePlayer(PlayerColor pc, TGMGame* game, bool outputHowTheComputerWouldDo)
{
	TGMPlayer* player = CreatePlayer("You", Strategy_Human, pc);
	s_outputHowTheComputerWouldDo = outputHowTheComputerWouldDo;
	
	PlayerSetOnPlayInGame(player, &PlayInGame);
	
	GameSetCallbackContext(game, player);
	GameSetOnPlayerDidPlay(game, &PlayedMove);
	GameSetOnStartedHand(game, &HandStarted);
	GameSetOnStartedTurn(game, &TurnStarted);
	GameSetOnEndedTurn(game, &TurnEnded);
	GameSetOnDidStart(game, &GameStarted);
	GameSetOnDidFinish(game, &GameFinished);
	GameSetOnPlayerDidKillPlayer(game, &PlayerKilledPlayer);
	
	return player;
}

void DisplayHand(TGMCardList* hand, bool isSelf)
{
	if (isSelf)
	{
		printf("Your hand:\n");
	}
	
	for (unsigned c = 0; c < CardListCount(hand); c++)
	{
		TGMCard* card = CardAtIndex(hand, c);
		printf("\t%u) %s\n", c + 1, CardDescription(card, true).c_str());
		ReleaseCard(card);
	}
}

void PlayInGame(TGMPlayer* player)
{
	TGMGame *game = PlayerGetGame(player);
	printf("%s\n", BoardDescription(GameGetBoard(game)).c_str());
	
	while (true)
	{
		DisplayHand(PlayerGetHand(player), true);
		
		int optionCount = (int)CardListCount(PlayerGetHand(player));
		
		// TODO : Add discard option here if we know we have no more moves to play
		// TODO : Add option for saving game log to disk!
		// TODO : Add option for quitting!
		
		int index = ConsoleGetInputNumber("Enter card to play", 0, 1, optionCount);
		if (index == 0) continue;
		index--; // convert to real index
		TGMCard* card = CardAtIndex(PlayerGetHand(player), (unsigned)index);
		
		// check to see if any other cards have non-discard moves
		TGMMoveList* moves = MovesForPlayerSimple(player, game, card);
		unsigned movesCount = MoveListCount(moves);
		if (DoesPlayerHaveAnyNonDiscardMoves(player, card, moves, game))
		{
			movesCount = 0;
		}
		
		if (movesCount == 0)
		{
			printf("No valid moves with %s\n", CardDescription(card, true).c_str());
			ReleaseCard(card);
			continue;
		}
		
		{
			TGMMove* move = moves->first;
			int i = 1;
			while (move != nullptr)
			{
				printf("\t%d) %s\n", i++, MoveShortDescription(move, false).c_str());
				move = move->nextMove;
			}
		}
		
		printf("Playing %s\n", CardDescription(card, true).c_str());
		
		index = ConsoleGetInputNumber("Enter move to play", 0, 1, (int)MoveListCount(moves));
		if (index == 0)
		{
			ReleaseCard(card);
			continue;
		}
		
		index--; // convert to real index
		
		TGMMove* move = nullptr;
		{
			int i = 0;
			TGMMove* mv = moves->first;
			while (mv != nullptr)
			{
				if (i == index)
				{
					move = mv;
					break;
				}

				mv = mv->nextMove;
				++i;
			}
			
			dassert(move != nullptr);
		}
		
		SimulateGame(game, nullptr, true);
		
		printf("Playing %s\n", MoveShortDescription(move, true).c_str());
		GameDoMove(game, move);
		ReleaseMoveList(moves);
		ReleaseCard(card);
		break;
	}
}

void PlayedMove(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move)
{
	printf("%s played %s\n", ColoredString(PlayerGetColor(player), PlayerGetName(player)).c_str(), MoveShortDescription(move, true).c_str());
	printf("%s\n", BoardDescription(GameGetBoard(game)).c_str());
}

void HandStarted(void* context, TGMGame* game)
{
	CardListSort(PlayerGetHand((TGMPlayer *)context));
}

void TurnStarted(void* context, TGMGame* game, TGMPlayer* player)
{
	printf("--> Turn %d started for %s\n", GameGetTurn(game), ColoredString(PlayerGetColor(player), PlayerGetName(player)).c_str());

#ifdef DEBUG
	if (player != context)
	{
		// printf("Hand before playing\n");
		// DisplayHand(PlayerGetHand(player), false);
	}
#endif
}

void TurnEnded(void* context, TGMGame* game, TGMPlayer* player)
{
#ifdef DEBUG
	if (player != context)
	{
		// printf("Hand after playing\n");
		// DisplayHand(PlayerGetHand(player), false);
	}
#endif

	printf("<-- Turn %d ended for %s\n", GameGetTurn(game), ColoredString(PlayerGetColor(player), PlayerGetName(player)).c_str());
#ifdef DEBUG
	ConsoleGetInput("Press enter to continue");
#else
	std::this_thread::sleep_for(std::chrono::seconds(1));
#endif

	printf("\n");
	printf("\n");
}

void GameStarted(void* context, TGMGame* game)
{
	printf("-- Game started!\n");
	
	printf("%s\n", BoardDescription(GameGetBoard(game)).c_str());
}

void GameFinished(void* context, TGMGame* game, bool team1Won)
{
	printf("-- Game finished!\n");
	TGMPlayer* consolePlayer = (TGMPlayer*)context;
	bool isTeam1 = PlayerIsTeam1(consolePlayer);
	if ((team1Won && isTeam1) || (!team1Won && !isTeam1))
	{
		printf("You won!\n");
	}
	else
	{
		printf("You lost...\n");
	}
}

void PlayerKilledPlayer(void* context, TGMGame* game, TGMPlayer* player, TGMPlayer* deadPlayer)
{
	printf("--Player %s killed %s\n",
		ColoredString(PlayerGetColor(player), PlayerGetName(player)).c_str(),
		ColoredString(PlayerGetColor(deadPlayer), PlayerGetName(deadPlayer)).c_str());
}

bool DoesPlayerHaveAnyNonDiscardMoves(TGMPlayer* player, TGMCard* card, TGMMoveList* moves, TGMGame* game)
{
	// allMoves only used to detect if we have valid non-discard moves
	TGMMoveList* allMoves = MovesForPlayerSimple(player, game, NULL);
		
	// detect if we have non-discard moves
	bool hasAnyNonDiscardMove = false;
	TGMMove* move = allMoves->first;
	while (move != nullptr)
	{
		if (!move->isDiscard)
		{
			hasAnyNonDiscardMove = true;
			break;
		}

		move = move->nextMove;
	}
		
	move = moves->first;
	bool hasNonDiscardMove = false;
	while (move != nullptr)
	{
		if (!move->isDiscard)
		{
			hasNonDiscardMove = true;
			break;
		}

		move = move->nextMove;
	}
	
	ReleaseMoveList(allMoves);
	
	// if we have a card that can be played that isn't a discard, but this card only has discard moves
	// don't let the player play
	return (hasAnyNonDiscardMove && !hasNonDiscardMove);
}

