//
//  GameLog.h
//  Marbles
//
//  Created by Todd on 1/8/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#pragma once
#include "Deck.h"
#include "DeckList.h"
#include "Move.h"
#include "MoveList.h"

#define TGMGameLog struct TGMGameLogStruct
// NOTE : versioned
struct TGMGameLogStruct
{
	TGMDeckList* _deckList;
	TGMMoveList* _moveList;
	
	int32_t _retainCount;
	PlayerColor _dealingPlayer;
	Strategy _playerStrategy[kPlayers];
	
	// TODO : do we want a write-only log and a readonly log?
	char _padding[6];
};

#define GameLogDealingPlayer(gameLog) gameLog->_dealingPlayer
#define GameLogSetDealingPlayer(gameLog, dealingPlayer) gameLog->_dealingPlayer = dealingPlayer

TGMGameLog* CreateGameLog(void);
void RetainGameLog(TGMGameLog* gameLog);
TGMGameLog* CopyGameLog(TGMGameLog* gameLog);
void ReleaseGameLog(TGMGameLog* gameLog);

void GameLogSetMoveList(TGMGameLog* gameLog, TGMMoveList* moveList);

Strategy GameLogStrategyForPlayerColor(TGMGameLog* gameLog, PlayerColor pc);
void GameLogSetStrategyForPlayerColor(TGMGameLog* gameLog, Strategy strategy, PlayerColor pc);

// for keeping track of the current game
void GameLogAddDeck(TGMGameLog* gameLog, TGMDeck* deck);
void GameLogAddMove(TGMGameLog* gameLog, TGMMove* move);

// for replaying a game
TGMDeck* GameLogPopDeck(TGMGameLog* gameLog); // owns reference
bool GameLogHasDeck(TGMGameLog* gameLog);
TGMMove* GameLogPopMove(TGMGameLog* gameLog); // owns reference
bool GameLogHasMove(TGMGameLog* gameLog);

bool AreGameLogsEqual(TGMGameLog* gameLog1, TGMGameLog* gameLog2);

