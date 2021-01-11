#include "precomp.h"
#include "DebugAssert.h"
#include "Game.h"
#include "GameLog.h"
#include "Board.h"
#include "Deck.h"
#include "Descriptions.h"
#include "Player.h"
#include "Logging.h"
#include "MarblesMem.h"
#include "Move.h"
#include "MoveList.h"
#include "Random.h"
#include "Spots.h"
#include "Thread.h"

void GetGameReady(TGMGame* game);
void GameStart(TGMGame* game);
void GameStartHand(TGMGame* game);
void GameDoPlay(TGMGame* game);
void GameGo(TGMGame* game, bool startNew);
void RunGame(TGMGame* game);
void ContinueGame(TGMGame* game);
bool GameEndTurn(TGMGame* game);
bool GameEndHand(TGMGame* game);
void GameEndGame(TGMGame* game);
bool GameIsEndOfHand(TGMGame* game);
bool GameIsEndOfGame(TGMGame* game, bool* team1Won);
void GameReturnCardsToDeck(TGMGame* game);
void GameConsistencyCheck(TGMGame* game);
uint8_t GameGetScoreForTeam(TGMGame* game, bool team1);

void GameEventHandStarting(TGMGame* game);
void GameEventGameStarted(TGMGame* game);
void GameEventTurnStarted(TGMGame* game);
void GameEventReplayLogEnded(TGMGame* game, TGMGameLog* gameLog);
void GameEventGameFinished(TGMGame* game, bool team1Won);
void GameEventPlayerWillPlayMove(TGMGame* game, TGMMove* move);
void GameEventPlayerKilledPlayer(TGMGame* game, TGMPlayer* deadPlayer);
void GameEventPlayerMoved(TGMGame* game, TGMMove* move);
void GameEventTurnEnded(TGMGame* game, TGMPlayer* oldPlayer);
void GameEventHandEnded(TGMGame* game);
void GameEventGameWillStart(TGMGame* game);
void GameEventPlayerDiscarded(TGMGame* game, TGMCard* card);


TGMGame* AllocateGame(void);
void DeallocateGame(TGMGame* game);

static std::mutex s_lock;
static TGMPoolAllocator* s_gameAllocationPoolUnusedFirst = nullptr;
static TGMPoolAllocator* s_gameAllocationPoolUsedFirst = nullptr;

TGMGame* AllocateGame(void)
{
	const std::lock_guard<std::mutex> lock(s_lock);
	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMGame), &s_gameAllocationPoolUnusedFirst, &s_gameAllocationPoolUsedFirst);
	TGMGame* game = static_cast<TGMGame *>(allocator->_object);
	game->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	LogMemoryCreate("GAME", game, MemIncreaseGlobalCount(g_gamesLiving));

	return game;
}

void DeallocateGame(TGMGame* game)
{
	const std::lock_guard<std::mutex> lock(s_lock);
	dassert(game);
	if (!game) return;
	
	for (int p = 0; p < kPlayers; p++)
	{
		TGMPlayer* player = game->_players[p];
		if (player != nullptr)
		{
			ReleasePlayer(player);
		}
	}
	
	ReleaseGameLog(game->_gameLog);
	ReleaseGameLog(game->_replayLog);
	DestroyBoard(game->_board);
	if (game->_deck) ReleaseDeck(game->_deck);
	DestroyRandom(game->_rng);
	
	TGMPoolAllocator* allocator = static_cast<TGMPoolAllocator*>(game->_holder); // need to do this before memset...
#ifdef DEBUG
	memset(game, static_cast<int>(0xDEADBEEF), sizeof(TGMGame));
	game->_retainCount = 0;
	game->_holder = allocator;
#endif
	
	DeallocatePoolObject(allocator, &s_gameAllocationPoolUnusedFirst, &s_gameAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_gamesLiving);
}

TGMGame* CreateGame(TGMGameLog* replayLog, TGRandom* rng)
{
	if (rng == nullptr)
	{
		rng = CreateRandomAndSeed();
	}
	
	TGMGame* game = AllocateGame();
	game->_retainCount = 1;
	memset(game->_players, 0, sizeof(game->_players));
	game->_rng = rng;
	game->_gameLog = CreateGameLog();
	game->_replayLog = replayLog;
	if (replayLog)
	{
		RetainGameLog(replayLog);
	}
	
	game->_state = GameState::NotStarted;
	game->_board = CreateBoard();
	game->_turn = 0;
	game->_dealingPlayer = PlayerColor::None;
	game->_currentPlayer = PlayerColor::None;
	game->_deck = nullptr;
	
	game->_callbackContext = nullptr;
	game->_onGameWillStart = nullptr;
	game->_onGameDidStart = nullptr;
	game->_onGameDidFinish = nullptr;
	game->_onGameDidFinishReplay = nullptr;
	game->_onStartedHand = nullptr;
	game->_onEndedHand = nullptr;
	game->_onStartedTurn = nullptr;
	game->_onEndedTurn = nullptr;
	game->_onPlayerWillPlay = nullptr;
	game->_onPlayerDidPlay = nullptr;
	game->_onPlayerDiscarded = nullptr;
	game->_onPlayerWillKillPlayer = nullptr;
	
	return game;
}

TGMGame* CopyGame(TGMGame* gameToCopy)
{
	if (gameToCopy == nullptr)
	{
		assert(false);
		return nullptr;
	}
	
	TGMGame* copyGame = AllocateGame();
	copyGame->_retainCount = 1;
	memset(copyGame->_players, 0, sizeof(copyGame->_players));
	
	copyGame->_rng = gameToCopy->_rng ? CopyRandom(gameToCopy->_rng) : nullptr;
	copyGame->_gameLog = gameToCopy->_gameLog ? CopyGameLog(gameToCopy->_gameLog) : nullptr;
	copyGame->_replayLog = gameToCopy->_replayLog ? CopyGameLog(gameToCopy->_replayLog) : nullptr;
	copyGame->_state = gameToCopy->_state;
	copyGame->_board = gameToCopy->_board ? CopyBoard(gameToCopy->_board) : nullptr;
	copyGame->_turn = gameToCopy->_turn;
	copyGame->_dealingPlayer = gameToCopy->_dealingPlayer;
	copyGame->_currentPlayer = gameToCopy->_currentPlayer;
	copyGame->_deck = gameToCopy->_deck ? CopyDeck(gameToCopy->_deck) : nullptr;
	
	copyGame->_callbackContext = nullptr;
	copyGame->_onGameWillStart = nullptr;
	copyGame->_onGameDidStart = nullptr;
	copyGame->_onGameDidFinish = nullptr;
	copyGame->_onGameDidFinishReplay = nullptr;
	copyGame->_onStartedHand = nullptr;
	copyGame->_onEndedHand = nullptr;
	copyGame->_onStartedTurn = nullptr;
	copyGame->_onEndedTurn = nullptr;
	copyGame->_onPlayerWillPlay = nullptr;
	copyGame->_onPlayerDidPlay = nullptr;
	copyGame->_onPlayerDiscarded = nullptr;
	copyGame->_onPlayerWillKillPlayer = nullptr;
	
	return copyGame;
}

TGMGame* RetainGame(TGMGame* game)
{
	if (game)
	{
		LogMemoryRetain("GAME", game, MemIncreaseRetainCount(game->_retainCount));
	}

	return game;
}

void ReleaseGame(TGMGame* game)
{
	if (!game) return;
	
	int32_t rc = MemDecreaseRetainCount(game->_retainCount);
	LogMemoryRelease("GAME", game, rc);

	dassert(rc >= 0);
	if (rc == 0)
	{
		DeallocateGame(game);
	}
}

void GameAddPlayer(TGMGame* game, TGMPlayer* player)
{
	PlayerColor pc = PlayerGetColor(player);
	dassert(game->_players[PositionForPlayerColor(pc)] == nullptr);
	game->_players[PositionForPlayerColor(pc)] = RetainPlayer(player);
	PlayerSetIsTeam1(player, IsPlayerColorTeam1(pc));
	
	GameLogSetStrategyForPlayerColor(game->_gameLog, PlayerGetStrategy(player), PlayerGetColor(player));
	
	PlayerSetGame(player, game);
	
}

TGMTeammates* GameTeammatesForPlayer(TGMGame* game, PlayerColor pc)
{
	dassert(kPlayers == 6); // doesn't handle more
	
	PlayerColor teammate1 = NextTeammateColor(pc);
	PlayerColor teammate2 = NextTeammateColor(teammate1);
	dassert(teammate1 != pc);
	dassert(teammate2 != pc);
	dassert(teammate1 != teammate2);
	
	return CreateTeammates(
		game->_players[PositionForPlayerColor(teammate1)],
		game->_players[PositionForPlayerColor(teammate2)],
		nullptr);
}

TGMTeammates* GameTeammatesIncludingPlayer(TGMGame* game, PlayerColor pc)
{
	dassert(kPlayers == 6); // doesn't handle more
	
	PlayerColor teammate1 = NextTeammateColor(pc);
	PlayerColor teammate2 = NextTeammateColor(teammate1);
	dassert(teammate1 != pc);
	dassert(teammate2 != pc);
	dassert(teammate1 != teammate2);
	return CreateTeammates(
		game->_players[PositionForPlayerColor(teammate1)],
		game->_players[PositionForPlayerColor(teammate2)],
		game->_players[PositionForPlayerColor(pc)]);
}

TGMPlayer* GamePlayerForColor(TGMGame* game, PlayerColor pc)
{
	return game->_players[PositionForPlayerColor(pc)];
}

void GameStartNewWithSeed(TGMGame* game, unsigned seed)
{
	if (game->_rng != nullptr)
	{
		DestroyRandom(game->_rng);
	}
	
	game->_rng = CreateRandomWithSeed(seed);
	GameStartNew(game);
}

void GameStartNew(TGMGame* game)
{
	dassert(game->_state == GameState::NotStarted || game->_state == GameState::GameOver);
	
	GameGo(game, true);
}

void GameContinue(TGMGame* game)
{
	GameGo(game, false);
}

static thread_local bool s_isGameThread {};

void GameGo(TGMGame* game, bool startNew)
{
	s_isGameThread = true;
	SetThreadName("Marbles Game Thread");
	
	if (startNew)
	{
		RunGame(game);
	}
	else
	{
		ContinueGame(game);
	}
	
	s_isGameThread = false;
	SetThreadName(nullptr);
}

bool IsGameThread()
{
	return s_isGameThread;
}

uint8_t GameGetTeam1Score(TGMGame* game)
{
	return GameGetScoreForTeam(game, true);
}

uint8_t GameGetTeam2Score(TGMGame* game)
{
	return GameGetScoreForTeam(game, false);
}

uint8_t GameGetScoreForTeam(TGMGame* game, bool team1)
{
	uint8_t score = 0;
	for (int pc = 0; pc < kPlayers; pc++)
	{
		TGMPlayer* player = game->_players[pc];
		if (PlayerIsTeam1(player) != team1)
		{
			continue;
		}
		
		score += BoardCountScoreForPlayer(game->_board, PlayerGetColor(player));
	}
	
	return score;
}

void RunGame(TGMGame* game)
{
	game->_state = GameState::NotStarted;
	ContinueGame(game);
}

void ContinueGame(TGMGame* game)
{
	while (game->_state != GameState::GameOver)
	{
		switch (game->_state)
		{
			case GameState::NotStarted:
				GetGameReady(game);
				game->_state = GameState::GameStarting;
				break;
				
			case GameState::GameStarting:
				GameStart(game);
				game->_state = GameState::HandStarting;
				break;
				
			case GameState::HandStarting:
				GameStartHand(game);
				game->_state = GameState::TurnStarting;
				break;
				
			case GameState::TurnStarting:
				game->_state = GameState::Playing;
				game->_turn++;
				GameEventTurnStarted(game);
				break;
				
			case GameState::Playing:
				GameDoPlay(game);
				game->_state = GameState::TurnEnding;
				break;
				
			case GameState::TurnEnding:
				{
					TGMPlayer* oldPlayer = GameGetCurrentPlayer(game);
					if (GameEndTurn(game))
					{
						game->_state = GameState::HandEnding;
					}
					else
					{
						game->_state = GameState::TurnStarting;
					}
					
					GameEventTurnEnded(game, oldPlayer);
				}
				break;
				
			case GameState::HandEnding:
				if (GameEndHand(game))
				{
					game->_state = GameState::GameEnding;
				}
				else
				{
					game->_state = GameState::HandStarting;
				}
				
				GameEventHandEnded(game);
				break;
				
			case GameState::GameEnding:
				GameEndGame(game);
				game->_state = GameState::GameOver;
				break;
				
			case GameState::GameOver:
				dassert(0); // shouldn't get here ever
				break;
		}
	}
}

void GetGameReady(TGMGame* game)
{
#ifdef DEBUG_LOGGING_ON
	TGMLogGameMessage(@"%p : Game started", game);
#endif
	ReleaseGameLog(game->_gameLog);
	
	game->_gameLog = CreateGameLog();
	for (int p = 0; p < kPlayers; p++)
	{
		TGMPlayer* player = game->_players[p];
		GameLogSetStrategyForPlayerColor(game->_gameLog, PlayerGetStrategy(player), PlayerGetColor(player));
	}
	
	game->_turn = 0;
	GameEventGameWillStart(game);
}

void GameStart(TGMGame* game)
{
	ResetBoard(game->_board);
	if (game->_deck != nullptr)
	{
		ReleaseDeck(game->_deck);
		game->_deck = nullptr;
	}
	
	for (int p = 0; p < kPlayers; p++)
	{
		TGMPlayer* player = game->_players[p];
		dassert(PlayerGetColor(player) == PlayerColorForPosition(p));
		PlayerGameStarting(player, game);
	}
	
	if (game->_currentPlayer == PlayerColor::None)
	{
		uint32_t randomNumber = RandomRandom(GameGetRandomNumberGenerator(game)); // setting the current player at game start
		uint32_t startingPosition = randomNumber % kPlayers;
		game->_currentPlayer = PlayerGetColor(game->_players[startingPosition]);
		dassert(game->_currentPlayer != PlayerColor::None);
	}
	
	GameEventGameStarted(game);
}

void GameStartHand(TGMGame* game)
{
	for (int pc = 0; pc < kPlayers; pc++)
	{
		TGMPlayer* player = game->_players[pc];
		PlayerStartingHand(player);
	}
	
	// see if the dealing player changed
	if (game->_deck == nullptr || DeckIsEmpty(game->_deck))
	{
		bool shuffle = true;
		if (game->_replayLog != nullptr && GameLogHasDeck(game->_replayLog))
		{
#ifdef DEBUG_LOGGING_ON
			TGMLogGameMessage(@"%p : Popped replay deck", game);
#endif	

			if (game->_deck) ReleaseDeck(game->_deck);
			game->_deck = GameLogPopDeck(game->_replayLog);
			shuffle = false;
		}
		
		if (game->_deck == nullptr)
		{
			game->_deck = CreateNonEmptyDeck();
		}
		
		if (shuffle)
		{
#ifdef DEBUG_LOGGING_ON
			TGMLogGameMessage(@"%p : Shuffling new deck", game);
#endif	
			
			DeckShuffle(game->_deck, game->_rng);
		}
		
		GameLogAddDeck(game->_gameLog, game->_deck);
		
		if (game->_dealingPlayer == PlayerColor::None)
		{
			uint32_t randomNumber = RandomRandom(game->_rng); // setting the dealing player at start
			if (game->_replayLog != nullptr && GameLogDealingPlayer(game->_replayLog) != PlayerColor::None)
			{
				game->_dealingPlayer = GameLogDealingPlayer(game->_replayLog);
			}
			else
			{
				uint32_t dealerPosition = randomNumber % kPlayers;
				game->_dealingPlayer = PlayerColorForPosition(dealerPosition);
				dassert(game->_dealingPlayer >= PlayerColor::Min && game->_dealingPlayer <= PlayerColor::Max);
			}
			
			GameLogSetDealingPlayer(game->_gameLog, game->_dealingPlayer);
		}
		else
		{
			game->_dealingPlayer = NextPlayerColor(game->_dealingPlayer);
		}
	}
	
	// set the current player
	GameMoveCurrentPlayerToNextPlayer(game, game->_dealingPlayer);
	
	// deal
	for (int cc = 0; cc < kCardsPerHand; cc++)
	{
		for (int pc = 0; pc < kPlayers; pc++)
		{
			TGMPlayer* player = game->_players[pc];
			TGMCard* card = DeckRemoveTopCard(game->_deck); // owns reference
			dassert(card != nullptr);
			PlayerDealtCard(player, card);
			ReleaseCard(card);
		}
	}
	
#ifdef DEBUG_LOGGING_ON
	TGMLogGameMessage(@"Dealer : %s", PlayerColorToString(game->_dealingPlayer));
	for (int pc = 0; pc < kPlayers; pc++)
	{
		TGMPlayer* player = game->_players[pc];
		TGMLogGameMessage(PlayerColorToString(PlayerGetColor(player)));
		TGMLogGameMessage(CardListDescription(PlayerGetHand(player), YES));
	}
#endif
	
	GameEventHandStarting(game);
}

void GameDoPlay(TGMGame* game)
{
	bool replayMoveIsValid = false;
	if (game->_replayLog != nullptr && GameLogHasMove(game->_replayLog))
	{
		bool replayLogEnded = false;
		
		// note that the marble the move points at may not be the same marble
		// as the one in this game -- so we need to fix that up here -- but we
		// can't use the AreMarblesEqual because the move may have modified the
		// marble's attributes - so we look just by color
		TGMMove* replayMove = GameLogPopMove(game->_replayLog);
		if (!replayMove->marble)
		{
			replayMoveIsValid = true;
		}
		else
		{
			int spot = BoardFindSpotWithMarbleColor(game->_board, replayMove->marble->color);
			dassert(spot != -1);
			if (spot != -1)
			{
				ReleaseMarble(replayMove->marble);
				replayMove->marble = BoardMarbleAtSpot(game->_board, spot);
				replayMoveIsValid = true;
			}
		}
		
		if (replayMoveIsValid)
		{
			replayLogEnded = !GameLogHasMove(game->_replayLog);
			GameDoMove(game, replayMove);
		}
		else
		{
			// dont' allow further replaying since we're deviating from this spot
			replayLogEnded = true;
		}
		
		ReleaseMove(replayMove);
		
		if (replayLogEnded)
		{
			GameEventReplayLogEnded(game, game->_replayLog);
			ReleaseGameLog(game->_replayLog);
			game->_replayLog = nullptr;
		}
	}
	
	if (!replayMoveIsValid)
	{
		PlayerPlayInGame(GameGetCurrentPlayer(game));
	}
}

bool GameEndTurn(TGMGame* game)
{
	bool endOfHand = GameIsEndOfHand(game) || GameIsEndOfGame(game, nullptr);
	if (!endOfHand)
	{
		// move to next player
		GameMoveCurrentPlayerToNextPlayer(game, game->_currentPlayer);
	}
	
	// debug check
#ifdef DEBUG
	GameConsistencyCheck(game);
#endif
	
	return endOfHand;
}

bool GameEndHand(TGMGame* game)
{
	GameReturnCardsToDeck(game);
	
	bool endOfGame = GameIsEndOfGame(game, nullptr);
	return endOfGame;
}

void GameEndGame(TGMGame* game)
{
	for (int pc = 0; pc < kPlayers; pc++)
	{
		TGMPlayer* player = game->_players[pc];
		PlayerGameEnding(player, game);
	}
	
	bool team1Won = false;
	GameIsEndOfGame(game, &team1Won);
	
	GameReturnCardsToDeck(game);
	ReleaseDeck(game->_deck);
	game->_deck = nullptr;
	
	GameEventGameFinished(game, team1Won);
}

bool GameIsEndOfHand(TGMGame* game)
{
	for (int pc = 0; pc < kPlayers; pc++)
	{
		TGMPlayer* player = game->_players[pc];
		if (CardListCount(PlayerGetHand(player)) != 0)
		{
			return false;
		}
	}
	
	return true;
}

bool GameIsEndOfGame(TGMGame* game, bool* team1Won)
{
	// look to see if everybody on one team
	bool team1Finished = true;
	bool team2Finished = true;
	
	for (PlayerColor pc = PlayerColor::Min; pc <= PlayerColor::Max; IteratePlayerColor(pc))
	{
		if (BoardCountMarblesInFinalSpotForPlayer(game->_board, pc) != kMarblesPerPlayer)
		{
			if (IsPlayerColorTeam1(pc))
			{
				team1Finished = false;
			}
			else
			{
				team2Finished = false;
			}
		}
		
		// early out
		if (!team1Finished && !team2Finished)
		{
			return false;
		}
	}
	
	bool isEnd = team1Finished || team2Finished;
	if (isEnd && team1Won != nullptr)
	{
		*team1Won = team1Finished;
	}
	
	return isEnd;
}

void GameDoMove(TGMGame* game, TGMMove* move)
{
#ifdef DEBUG_LOGGING_ON
	TGMLogGameMessage(@"Turn %d : %@", game->_turn, MoveDescription(move));
#endif
	
	dassert(move != nullptr);
	GameEventPlayerWillPlayMove(game, move);
	
	// remove the card from the player's hand
	TGMCard *playingCard = nullptr;
	if (game->_currentPlayer != PlayerColor::None) // _currentPlayer can be PlayerColor::None in unit tests
	{
		TGMCardList *hand = PlayerGetHand(GameGetCurrentPlayer(game));
		playingCard = CardListRemoveCardLike(hand, move->card); // owns reference
		dassert(playingCard != nullptr);
		DeckAddDiscard(game->_deck, playingCard);
		ReleaseCard(playingCard);
	}
	
	GameLogAddMove(game->_gameLog, move);
	if (move->marble != nullptr)
	{
		// TODO : put this logic in the move?
		if (move->oldSpot != kGetOutSpot)
		{
			move->marble->distanceFromHome += (move->newSpot - move->oldSpot);
		}
		else
		{
			move->marble->distanceFromHome += kPlayerStartingPosition;
		}
		
		// in the case where we've gone from 2 to -2, we want to mark it as moved around 106.
		move->marble->distanceFromHome = (move->marble->distanceFromHome + kTotalSpots) % kTotalSpots;
		
		// we only mark it YES as it's not a changeable value once set (unless killed - 
		// at which point we recreate the marble)
		if (move->wentBehindHome)
		{
			move->marble->wentBehindHome = move->wentBehindHome;
		}
	}
	
	if (!move->isDiscard)
	{
		// move the piece on the board
		if (!IsFinalSpot(move->newSpot))
		{
			MarbleColor existingColor = BoardMarbleColorAtSpot(game->_board, move->newSpot);
			bool isKill = (existingColor != MarbleColor::None);
			if (isKill)
			{
				// now remove the marble
				TGMPlayer* player = game->_players[PositionForPlayerColor(PlayerColorForMarbleColor(existingColor))];
				BoardRemoveMarble(game->_board, move->newSpot);
				PlayerRestoreMarble(player, existingColor);
				GameEventPlayerKilledPlayer(game, player);
			}
		}
		
		if (IsFinalSpot(move->newSpot))
		{
			if (IsFinalSpot(move->oldSpot))
			{
				BoardMoveMarbleAtFinalSpot(game->_board, move->playerColor, SpotToFinalSpot(move->oldSpot), SpotToFinalSpot(move->newSpot));
			}
			else
			{
				MarbleColor marbleColor = BoardMarbleColorAtSpot(game->_board, move->oldSpot);
				PlayerColor playerColor = PlayerColorForMarbleColor(marbleColor);
				BoardRemoveMarble(game->_board, move->oldSpot);
				BoardPlaceMarbleAtFinalSpot(game->_board, marbleColor, playerColor, SpotToFinalSpot(move->newSpot));
			}
		}
		else
		{
			if (move->oldSpot != kGetOutSpot)
			{
				// regular move
				BoardMoveMarble(game->_board, static_cast<int8_t>(move->oldSpot), static_cast<int8_t>(move->newSpot));
			}
			else
			{
				// get out move
				// TODO : this doesn't handle team member get-outs
				dassert(PlayerStartingSpot(move->playerColor) == move->newSpot);
				
				TGMMarble* marble = CreateMarble(
					PlayerRemoveFirstUnusedMarbleColor(game->_players[PositionForPlayerColor(move->playerColor)]),
					kPlayerStartingPosition,
					false);
				BoardPlaceMarble(game->_board, marble, move->newSpot);
				ReleaseMarble(marble);
			}
		}
	}
	else
	{
		GameEventPlayerDiscarded(game, playingCard);
	}
	
	GameEventPlayerMoved(game, move);
}

void GameMoveCurrentPlayerToNextPlayer(TGMGame* game, PlayerColor pc)
{
	PlayerColor nextPlayerColor = NextPlayerColor(pc);
	int nextPos = PositionForPlayerColor(nextPlayerColor);
	game->_currentPlayer = PlayerGetColor(game->_players[nextPos]);
	dassert(game->_currentPlayer != PlayerColor::None);
}

void GameReturnCardsToDeck(TGMGame* game)
{
	// return all cards back to the deck
	for (int pc = 0; pc < kPlayers; pc++)
	{
		TGMPlayer* player = game->_players[pc];
		TGMCardList* cards = PlayerGetHand(player);
		TGMCard *card = FirstCardNoRef(cards);
		while (card != nullptr)
		{
			TGMCard *next = card->nextCard;
			RetainCard(card);
			CardListRemove(cards, card);
			DeckAddDiscard(game->_deck, card);
			ReleaseCard(card);
			card = next;
		}
	}
}

void GameConsistencyCheck(TGMGame* game)
{
	for (int pc = 0; pc < kPlayers; pc++)
	{
		TGMPlayer* player = game->_players[pc];
		PlayerColor color = PlayerGetColor(player);
		
		int count = 0;
		for (int i = 0; i < kTotalSpots; i++)
		{
			if (IsPlayersMarble(BoardMarbleColorAtSpot(game->_board, i), color))
			{
				count++;
			}
		}
		
		for (int i = 0; i < kMarblesPerPlayer; i++)
		{
			if (BoardMarbleColorInFinalSpot(game->_board, i, color) != MarbleColor::None)
			{
				count++;
			}
		}
		
		int unusedMarbleColorCount = PlayerUnusedMarbleCount(player);
		if (count + unusedMarbleColorCount != kMarblesPerPlayer)
		{
			printf("Found %d marbles on board + %d unused for color %s.\n", count, unusedMarbleColorCount, PlayerColorToString(color));
			printf("Consistency check failed for %s!\n", PlayerDescription(player).c_str());
			printf("Board = \n%s\n", BoardDescription(game->_board).c_str());
			dassert(false);
		}
	}
}

#define CallGameEvent(evt, ...) if (game->evt != nullptr) (*game->evt)(game->_callbackContext, game, ##__VA_ARGS__);

void GameSetCallbackContext(TGMGame* game, void* callbackContext) { game->_callbackContext = callbackContext; }
void GameSetOnWillStart(TGMGame* game, TGMGameWillStartFunction onGameWillStart) { game->_onGameWillStart = onGameWillStart; }
void GameSetOnDidStart(TGMGame* game, TGMGameDidStartFunction onGameDidStart) { game->_onGameDidStart = onGameDidStart; }
void GameSetOnDidFinish(TGMGame* game, TGMGameDidFinishFunction onGameDidFinish) { game->_onGameDidFinish = onGameDidFinish; }
void GameSetOnDidFinishReplay(TGMGame* game, TGMGameDidFinishReplayFunction onGameDidFinishReplay) { game->_onGameDidFinishReplay = onGameDidFinishReplay; }
void GameSetOnStartedHand(TGMGame* game, TGMGameStartedHandFunction onStartedHand) { game->_onStartedHand = onStartedHand; }
void GameSetOnEndedHand(TGMGame* game, TGMGameEndedHandFunction onEndedHand) { game->_onEndedHand = onEndedHand; }
void GameSetOnStartedTurn(TGMGame* game, TGMGameStartedTurnFunction onStartedTurn) { game->_onStartedTurn = onStartedTurn; }
void GameSetOnEndedTurn(TGMGame* game, TGMGameEndedTurnFunction onEndedTurn) { game->_onEndedTurn = onEndedTurn; }
void GameSetOnPlayerWillPlay(TGMGame* game, TGMGamePlayerWillPlayFunction onPlayerWillPlay) { game->_onPlayerWillPlay = onPlayerWillPlay; }
void GameSetOnPlayerDidPlay(TGMGame* game, TGMGamePlayerDidPlayFunction onPlayerDidPlay) { game->_onPlayerDidPlay = onPlayerDidPlay; }
void GameSetOnPlayerDiscarded(TGMGame* game, TGMGamePlayerDiscardedFunction onPlayerDiscarded) { game->_onPlayerDiscarded = onPlayerDiscarded; }
void GameSetOnPlayerDidKillPlayer(TGMGame* game, TGMGamePlayerDidKillPlayerFunction onPlayerWillKillPlayer) { game->_onPlayerWillKillPlayer = onPlayerWillKillPlayer; }


void GameEventHandStarting(TGMGame* game) { CallGameEvent(_onStartedHand); }
void GameEventGameStarted(TGMGame* game) { CallGameEvent(_onGameDidStart); }
void GameEventTurnStarted(TGMGame* game) { CallGameEvent(_onStartedTurn, GameGetCurrentPlayer(game)); }
void GameEventReplayLogEnded(TGMGame* game, TGMGameLog* gameLog) { CallGameEvent(_onGameDidFinishReplay, gameLog); }
void GameEventGameFinished(TGMGame* game, bool team1Won) { CallGameEvent(_onGameDidFinish, team1Won); }
void GameEventPlayerWillPlayMove(TGMGame* game, TGMMove* move) { CallGameEvent(_onPlayerWillPlay, GameGetCurrentPlayer(game), move); }
void GameEventPlayerKilledPlayer(TGMGame* game, TGMPlayer* deadPlayer) { CallGameEvent(_onPlayerWillKillPlayer, GameGetCurrentPlayer(game), deadPlayer); }
void GameEventPlayerMoved(TGMGame* game, TGMMove* move) { CallGameEvent(_onPlayerDidPlay, GameGetCurrentPlayer(game), move); }
void GameEventTurnEnded(TGMGame* game, TGMPlayer* oldPlayer) { CallGameEvent(_onEndedTurn, oldPlayer); }
void GameEventHandEnded(TGMGame* game) { CallGameEvent(_onEndedHand); }
void GameEventGameWillStart(TGMGame* game) { CallGameEvent(_onGameWillStart); }
void GameEventPlayerDiscarded(TGMGame* game, TGMCard* card) { CallGameEvent(_onPlayerDiscarded, GameGetCurrentPlayer(game), card); }
