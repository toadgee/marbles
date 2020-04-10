//
//  Random.h
//  Marbles
//
//  Created by Todd on 1/21/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#pragma once

#define TGRandom struct TGRandomStruct
struct TGRandomStruct
{
	unsigned _seed;
	unsigned _calls;
};

TGRandom* CreateRandomAndSeed(void);
TGRandom* CreateRandomWithSeed(unsigned seed);
void DestroyRandom(TGRandom* random);

void RestoreRandom(TGRandom* random);
void ResetRandom(TGRandom* random);
TGRandom* CopyRandom(TGRandom* random);

uint32_t RandomRandom(TGRandom* random);

#define RandomSeed(random) random->_seed
#define RandomCalls(random) random->_calls

