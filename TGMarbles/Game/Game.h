//
//	Game.h
//	Marbles
//
//	Created by todd harris on 11/27/11.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#pragma once
#include "Board.h"
#include "Card.h"
#include "Deck.h"
#include "GameLog.h"
#include "GameState.h"
#include "Move.h"
#include "MoveList.h"
#include "Random.h"
#include "Teammates.h"

#define kCardsPerHand			6 // number of cards dealt to each player

struct TGMPlayerStruct;
struct TGMGameStruct;
#define TGMGame struct TGMGameStruct

typedef void (*TGMGameWillStartFunction)(void* context, TGMGame* game);
typedef void (*TGMGameDidStartFunction)(void* context, TGMGame* game);
typedef void (*TGMGameDidFinishFunction)(void* context, TGMGame* game, bool team1Won);
typedef void (*TGMGameDidFinishReplayFunction)(void* context, TGMGame* game, TGMGameLog* gameLog);
typedef void (*TGMGameStartedHandFunction)(void* context, TGMGame* game);
typedef void (*TGMGameEndedHandFunction)(void* context, TGMGame* game);
typedef void (*TGMGameStartedTurnFunction)(void* context, TGMGame* game, struct TGMPlayerStruct* player);
typedef void (*TGMGameEndedTurnFunction)(void* context, TGMGame* game, struct TGMPlayerStruct* player);
typedef void (*TGMGamePlayerWillPlayFunction)(void* context, TGMGame* game, struct TGMPlayerStruct* player, TGMMove* move);
typedef void (*TGMGamePlayerDidPlayFunction)(void* context, TGMGame* game, struct TGMPlayerStruct* player, TGMMove* move);
typedef void (*TGMGamePlayerDiscardedFunction)(void* context, TGMGame* game, struct TGMPlayerStruct* player, TGMCard* card);
typedef void (*TGMGamePlayerDidKillPlayerFunction)(void* context, TGMGame* game, struct TGMPlayerStruct* player, struct TGMPlayerStruct* deadPlayer);

struct TGMGameStruct
{
	int32_t _retainCount;
	void* _holder;
	
	void* _callbackContext;
	
	TGMGameWillStartFunction _onGameWillStart;
	TGMGameDidStartFunction _onGameDidStart;
	TGMGameDidFinishFunction _onGameDidFinish;
	TGMGameDidFinishReplayFunction _onGameDidFinishReplay;
	
	TGMGameStartedHandFunction _onStartedHand;
	TGMGameEndedHandFunction _onEndedHand;
	
	TGMGameStartedTurnFunction _onStartedTurn;
	TGMGameEndedTurnFunction _onEndedTurn;
		
	TGMGamePlayerWillPlayFunction _onPlayerWillPlay;
	TGMGamePlayerDidPlayFunction _onPlayerDidPlay;
	TGMGamePlayerDiscardedFunction _onPlayerDiscarded;
	TGMGamePlayerDidKillPlayerFunction _onPlayerWillKillPlayer;
	
	struct TGMPlayerStruct* _players[kPlayers];
	
	int _turn;
	
	PlayerColor _dealingPlayer;
	PlayerColor _currentPlayer;
	
	GameState _state;
	TGMBoard* _board;
	TGMDeck* _deck;
	
	TGRandom* _rng;
	
	TGMGameLog* _replayLog; // replaying these moves
	TGMGameLog* _gameLog; // the moves we've done
};

TGMGame* RetainGame(TGMGame* game);
void ReleaseGame(TGMGame* game);


TGMGame* CreateGame(TGMGameLog* replayLog, TGRandom* rng);
TGMGame* CopyGame(TGMGame* game);

void GameAddPlayer(TGMGame* game, struct TGMPlayerStruct* player);
TGMTeammates* GameTeammatesForPlayer(TGMGame* game, PlayerColor pc);
TGMTeammates* GameTeammatesIncludingPlayer(TGMGame* game, PlayerColor pc);
struct TGMPlayerStruct* GamePlayerForColor(TGMGame* game, PlayerColor pc);

void GameStartNewWithSeed(TGMGame* game, unsigned seed);
void GameStartNew(TGMGame* game);
void GameContinue(TGMGame* game);
void GameDoMove(TGMGame* game, TGMMove* move);
bool IsGameThread();
void GameMoveCurrentPlayerToNextPlayer(TGMGame* game, PlayerColor pc);

uint8_t GameGetTeam1Score(TGMGame* game);
uint8_t GameGetTeam2Score(TGMGame* game);

void GameSetCallbackContext(TGMGame* game, void* callbackContext);
void GameSetOnWillStart(TGMGame* game, TGMGameWillStartFunction onGameWillStart);
void GameSetOnDidStart(TGMGame* game, TGMGameDidStartFunction onGameDidStart);
void GameSetOnDidFinish(TGMGame* game, TGMGameDidFinishFunction onGameDidFinish);
void GameSetOnDidFinishReplay(TGMGame* game, TGMGameDidFinishReplayFunction onGameDidFinishReplay);
void GameSetOnStartedHand(TGMGame* game, TGMGameStartedHandFunction onStartedHand);
void GameSetOnEndedHand(TGMGame* game, TGMGameEndedHandFunction onEndedHand);
void GameSetOnStartedTurn(TGMGame* game, TGMGameStartedTurnFunction onStartedTurn);
void GameSetOnEndedTurn(TGMGame* game, TGMGameEndedTurnFunction onEndedTurn);
void GameSetOnPlayerWillPlay(TGMGame* game, TGMGamePlayerWillPlayFunction onPlayerWillPlay);
void GameSetOnPlayerDidPlay(TGMGame* game, TGMGamePlayerDidPlayFunction onPlayerDidPlay);
void GameSetOnPlayerDiscarded(TGMGame* game, TGMGamePlayerDiscardedFunction onPlayerDiscarded);
void GameSetOnPlayerDidKillPlayer(TGMGame* game, TGMGamePlayerDidKillPlayerFunction onPlayerWillKillPlayer);

#define GameGetBoard(game) game->_board
#define GameGetTurn(game) game->_turn
#define GameGetGameLog(game) game->_gameLog
#define GameGetRandomNumberGenerator(game) game->_rng
#define GameGetCurrentPlayer(game) GamePlayerForColor(game, game->_currentPlayer)
#define GameGetPlayerAtIndex(game, i) game->_players[i]
