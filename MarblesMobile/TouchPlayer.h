//
//  TouchPlayer.h
//  Marbles
//
//  Created by toddha on 12/17/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#include "Player.h"
#include "Move.h"
#include "Game.h"

TGMPlayer* CreateTouchPlayer(const char* name, PlayerColor pc, TGMGame* game);

BOOL TouchPlayerGetAutomaticallyDiscardWhenItsOurTurn(TGMPlayer* player);
void TouchPlayerSetAutomaticallyDiscardWhenItsOurTurn(TGMPlayer* player, BOOL automaticallyDiscardWhenItsOurTurn);

void TouchPlayerSetMove(TGMPlayer* player, TGMMove* move);
BOOL TouchPlayerIsMyTurn(TGMPlayer *player);
void TouchPlayerFinishedPlaying(TGMPlayer* player);
