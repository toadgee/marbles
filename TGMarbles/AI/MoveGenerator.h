//
//  MoveGenerator.h
//  Marbles
//
//  Created by todd harris on 7/22/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#pragma once
#include "MoveList.h"
#include "Move.h"
#include "Board.h"
#include "Card.h"
#include "CardList.h"
#include "Teammates.h"
#include "Player.h"
#include "Game.h"

TGMMoveList* AllPossibleMovesForAllTeamPlayers(TGMGame* game, bool team1);

TGMMoveList* MovesForPlayerSimple(TGMPlayer* player, TGMGame *game, TGMCard* card);
TGMMoveList* MovesForPlayer(TGMPlayer* player, TGMCardList* hand, TGMTeammates* teammates, TGMBoard* board, TGMCard* card, TGMGame* game);
TGMMove* BestDiscardMoveInGameFromMoves(TGMMoveList *discardMoves);
TGMMove* BestMoveFromMoves(TGMMoveList* moves, TGMGame* game, PlayerColor pc, Strategy strategy);
