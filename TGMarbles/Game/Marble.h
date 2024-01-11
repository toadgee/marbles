//
//  Marble.h
//  Marbles
//
//  Created by toddha on 7/18/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#pragma once

#include "MarbleColor.h"

#define kMarblesPerPlayer		5 // number of marbles per player

#define TGMMarble struct TGMMarbleStruct
// NOTE : versioned
struct TGMMarbleStruct
{
	void*       _holder;
	int			_retainCount;
	MarbleColor color;
	int16_t     distanceFromHome;
	bool        wentBehindHome;
	char        _padding[7];
};

TGMMarble* CreateMarble(MarbleColor color, int16_t distanceFromHome, bool wentBehindHome);
TGMMarble* CopyMarble(TGMMarble* marble);

bool AreMarblesEqual(TGMMarble* m1, TGMMarble* m2);

#ifdef DEBUG
void RetainMarble(TGMMarble* marble);
void ReleaseMarble(TGMMarble* marble);
#else
#define RetainMarble(marble) MemIncreaseRetainCount(marble->_retainCount)
#define ReleaseMarble(marble) if (MemDecreaseRetainCount(marble->_retainCount) == 0) DeallocateMarble(marble)
#endif
void DeallocateMarble(TGMMarble* marble);
