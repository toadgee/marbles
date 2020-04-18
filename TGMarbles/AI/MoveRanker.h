//
//  MoveRanker.h
//  Marbles
//
//  Created by todd harris on 7/22/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#pragma once
#include "Move.h"
#include "MoveList.h"

int CalculateWeightOfMoveInGame(
	TGMMove* move, 
	TGMGame* game, 
	PlayerColor pc, 
	Strategy strategy,
	TGMMoveList** allPossibleOpponentMoves);
