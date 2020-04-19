//
//  MoveTests.h
//  Marbles
//
//  Created by Todd on 9/11/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import <XCTest/XCTest.h>
#include "Marble.h"
#include "Board.h"
#include "CardList.h"
#include "MarblesMem.h"
#include "Player.h"
#include "Game.h"

@interface MoveTests : XCTestCase
{
	TGMCardList* _hand;
	TGMGame* _game;
	TGMBoard* _board;
	TGMPlayer* _player;
	TGMPlayer* _teammate1;
	
	PlayerColor _playerColor;
	PlayerColor _teammate1Color;

#ifdef DEBUG
	MemorySnapshot _mem;
#endif
}

@end
