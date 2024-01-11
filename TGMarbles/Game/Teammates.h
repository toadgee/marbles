//
//  Teammates.h
//  Marbles
//
//  Created by Todd on 6/10/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#pragma once
#include "Player.h"

#define TGMTeammates struct TGMTeammatesStruct
struct TGMTeammatesStruct
{
	TGMPlayer* _teammate1;
	TGMPlayer* _teammate2;
	TGMPlayer* _ownPlayer;
	
	void *_holder;
	
	int32_t _count;
	char _padding[4];
};

TGMTeammates* CreateTeammates(
	TGMPlayer* teammate1,
	TGMPlayer* teammate2,
	TGMPlayer* ownPlayer);
TGMTeammates* CreateEmptyTeammates(void);

TGMPlayer* TeammatesPlayer(TGMTeammates *teammates, int32_t i);
void DeallocateTeammates(TGMTeammates* teammates);

#define TeammatesCount(tm) tm->_count
