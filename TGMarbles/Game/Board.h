//
//  Board.h
//  Marbles
//
//  Created by todd harris on 11/26/11.
//  Copyright 2012 toadgee.com. All rights reserved.
//

#pragma once
#include "Marble.h"
#include "Player.h"
#include "Spots.h"

#define TGMBoard struct TGMBoardStruct
struct TGMBoardStruct
{
	TGMMarble* _board[kTotalSpots];

	// Keeps track of the final spots for a player's marble.
	MarbleColor _finalSpots[kPlayers][kMarblesPerPlayer];
	uint8_t _finalSpotCounts[kPlayers];

	// keeps track of where all the players marbles are
	int8_t _playerMarbles[kPlayers][kMarblesPerPlayer];
	int8_t _playerMarblesInPlay[kPlayers];
};

TGMBoard* CreateBoard(void);
void DestroyBoard(TGMBoard *board);

TGMBoard* CopyBoard(TGMBoard *board);

void ResetBoard(TGMBoard *board);

// NOTE : this does not take final spots into account
int8_t BoardCountMarblesInPlayForPlayer(TGMBoard *board, PlayerColor pc);

TGMMarble *BoardMarbleAtSpot(TGMBoard *board, int spot); // retains return value
MarbleColor BoardMarbleColorAtSpot(TGMBoard *board, int spot);
MarbleColor BoardMarbleColorInFinalSpot(TGMBoard *board, int finalSpot, PlayerColor pc);
int BoardFirstMarbleInFinalSpots(TGMBoard *board, int startingSpot, PlayerColor pc);

void BoardPlaceMarble(TGMBoard* board, TGMMarble* marble, int spot);
void BoardRemoveMarble(TGMBoard* board, int spot);
void BoardMoveMarble(TGMBoard* board, int8_t oldSpot, int8_t newSpot);

uint8_t BoardCountScoreForPlayer(TGMBoard *board, PlayerColor pc);
uint8_t BoardCountMarblesInFinalSpotForPlayer(TGMBoard *board, PlayerColor pc);
void BoardPlaceMarbleAtFinalSpot(TGMBoard* board, MarbleColor mc, PlayerColor pc, int finalSpot);
void BoardRemoveMarbleAtFinalSpot(TGMBoard* board, PlayerColor pc, int finalSpot);
void BoardMoveMarbleAtFinalSpot(TGMBoard* board, PlayerColor pc, int oldFinalSpot, int newFinalSpot);

int8_t BoardFindSpotWithPlayerColor(TGMBoard* board, PlayerColor pc, int initialSpot);
int8_t BoardFindSpotWithMarbleColor(TGMBoard* board, MarbleColor mc);

bool AreBoardsEqual(TGMBoard *board, TGMBoard *other);

int8_t SlowInternalGetUnusedMarbleColorCountForPlayer(TGMBoard *board, PlayerColor pc);
