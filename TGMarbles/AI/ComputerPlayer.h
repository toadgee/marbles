//
//  ComputerPlayer.h
//  Marbles
//
//  Created by todd harris on 11/27/11.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#pragma once
#include "Player.h"

TGMPlayer* CreateComputerPlayer(const char* name, Strategy strategy, PlayerColor pc);

void ComputerPlayerPlayInGameWithStrategy(TGMPlayer* player, Strategy s);
