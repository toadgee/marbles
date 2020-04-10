//
//	MarblesMem.h
//	Marbles
//
//	Created by todd harris on 4/13/13.
//	Copyright 2013 toadgee.com. All rights reserved.
//

#pragma once

#include "PoolAllocator.h"

//#include <libkern/OSAtomic.h>

// retain count doesn't need atomic increment & decrement
#define MemIncreaseRetainCount(x) (++x)
#define MemDecreaseRetainCount(x) (--x)
//#define MemIncreaseRetainCount(x) OSAtomicIncrement32(&x)
//#define MemDecreaseRetainCount(x) OSAtomicDecrement32(&x)

#ifdef DEBUG
// global count doesn't need atomic increment & decrement right now
#define MemIncreaseGlobalCount(x) (++x)
#define MemDecreaseGlobalCount(x) (--x)
//#define MemIncreaseGlobalCount(x) OSAtomicIncrement64(&x)
//#define MemDecreaseGlobalCount(x) OSAtomicDecrement64(&x)
#else
#define MemIncreaseGlobalCount(x)
#define MemDecreaseGlobalCount(x)
#endif

//#define MEMORY_LOGGING 1
#ifdef MEMORY_LOGGING
#define LogMemoryCreate(typeName, pointer, GlobalIncreaseFunctionCall) NSLog(@"<<%s %p : %d (+) %lld>>", typeName, pointer, pointer->_retainCount, GlobalIncreaseFunctionCall);
#define LogMemoryRetain(typeName, pointer, RetainFunctionCall) NSLog(@"<<%s %p : %d (+)>>", typeName, pointer, RetainFunctionCall);
#define LogMemoryRelease(typeName, pointer, retainCount) NSLog(@"<<%s %p : %d (-)>>", typeName, pointer, retainCount);
#else
#define LogMemoryCreate(typeName, pointer,  GlobalIncreaseFunctionCall) GlobalIncreaseFunctionCall
#define LogMemoryRetain(typeName, pointer, RetainFunctionCall) RetainFunctionCall
#define LogMemoryRelease(typeName, pointer, ReleaseFunctionCall)
#endif

#ifdef DEBUG
extern int64_t g_cardCreateCount;
extern int64_t g_moveCreateCount;
extern int64_t g_marbleCreateCount;
extern int64_t g_moveListCreateCount;
extern int64_t g_cardListCreateCount;
extern int64_t g_cardListHolderCreateCount;
extern int64_t g_boardCreateCount;
extern int64_t g_deckCreateCount;
extern int64_t g_deckListCreateCount;
extern int64_t g_gameLogCreateCount;
extern int64_t g_teammatesCreateCount;
extern int64_t g_replayGamesCreateCount;
extern int64_t g_gamesCreateCount;
extern int64_t g_playersCreateCount;

extern int64_t g_boardsLiving;
extern int64_t g_cardListsLiving;
extern int64_t g_cardListHoldersLiving;
extern int64_t g_cardsLiving;
extern int64_t g_deckListsLiving;
extern int64_t g_decksLiving;
extern int64_t g_marblesLiving;
extern int64_t g_moveListsLiving;
extern int64_t g_movesLiving;
extern int64_t g_gameLogsLiving;
extern int64_t g_teammatesLiving;
extern int64_t g_replayGamesLiving;
extern int64_t g_gamesLiving;
extern int64_t g_playersLiving;

#define MemorySnapshot struct MemorySnapshotStruct
struct MemorySnapshotStruct
{
	int64_t _cardListsLiving;
	int64_t _cardListHoldersLiving;
	int64_t _moveListsLiving;
	int64_t _movesLiving;
	int64_t _marblesLiving;
	int64_t _cardsLiving;
	int64_t _boardsLiving;
	int64_t _decksLiving;
	int64_t _deckListsLiving;
	int64_t _gameLogsLiving;
	int64_t _teammatesLiving;
	int64_t _replayGamesLiving;
	int64_t _gamesLiving;
	int64_t _playersLiving;
};

MemorySnapshot CreateMemorySnapshot(void);
void MemoryCheck(MemorySnapshot snapshot);
#endif
