//
//	Move.h
//	Marbles
//
//	Created by todd harris on 11/27/11.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#pragma once
#include "Board.h"
#include "Card.h"
#include "Marble.h"
#include "Player.h"

#define TGMMove struct TGMMoveStruct
// NOTE : versioned
struct TGMMoveStruct
{
	TGMMove*	nextMove;
	TGMMove*	previousMove;
	
	void*       _holder;
	
	TGMMarble*	marble;
	TGMCard*	card;
	
	int 		_retainCount;
	int			oldSpot;				// from
	
	int			newSpot;				// to
	int			moves;					// the number of spots the marble will move
	
	int			jumps;					// how many jumps this move is
	PlayerColor playerColor;
	bool		isDiscard;				// whether or not the move is a discard
	bool		wentBehindHome;			// whether or not we can move out of the normalized spot of 0
	
	// not persisted : calculated on the fly
	int			weight;					// the weight (if calculated) of the move (not persisted)
	bool        weightCalculated;		// whether or not the weight is calculated (not persisted)
	uint8_t		spots[kMaxMoveSpots];   // this has every spot the marble is planning on going through (not persisted)
	bool		spotsCalculated;        // whether or not spots is detailed (not persisted)
	
	char _padding[4];
};

void MoveCalculateIntermediateSpots(TGMMove* move);

bool IsMoveValidOnBoard(TGMMove* move, TGMBoard* board, PlayerColor pc);
bool AreMovesEqual(TGMMove* move1, TGMMove* move2);
bool AreMovesEquivalent(TGMMove* move1, TGMMove* move2);

TGMMove* MakeMoveUnfiltered(
	TGMCard* card,
	TGMMarble* marble,
	PlayerColor playerColor,
	CardNumber cardNum,
	int moves,
	int oldSpot,
	int newSpot,
	int normalizedOldSpot,
	int jumps,
	int unusedMarbles);

TGMMove* MakeMove(
	TGMCard* card,
	TGMMarble* marble,
	bool isDiscard,
	PlayerColor playerColor,
	int oldSpot,
	int newSpot,
	int moves,
	int jumps,
	bool wentBehindHome);

TGMMove* CopyMove(TGMMove * move);
TGMMove* RetainMove(TGMMove *move);
void ReleaseMove(TGMMove* move);


