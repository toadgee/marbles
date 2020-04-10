//
//  ReplayGame.h
//  Marbles
//
//  Created by Todd on 2/1/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#pragma once
#include "GameLog.h"
#include "Game.h"

#define TGMReplayGame struct TGMReplayGameStruct
struct TGMReplayGameStruct
{
	int32_t _retainCount;
	void* _holder;
	TGMGame* _game;
};

TGMReplayGame* CreateReplayGame(TGMGameLog* gameLog, TGRandom* newRandom);
TGMReplayGame* RetainReplayGame(TGMReplayGame* replayGame);
void ReleaseReplayGame(TGMReplayGame* replayGame);

void ReplayGameGo(TGMReplayGame* replayGame);

#define ReplayGameGame(replayGame) replayGame->_game
