#include "precomp.h"
#include "ReplayGame.h"
#include "Board.h"
#include "DebugAssert.h"
#include "Descriptions.h"
#include "DumbPlayer.h"
#include "Game.h"
#include "MarblesMem.h"
#include "MoveList.h"
#include "MoveRanker.h"
#include "PoolAllocator.h"

TGMReplayGame* AllocateReplayGame(void);
void DeallocateReplayGame(TGMReplayGame* replayGame);

static TGMPoolAllocator* s_ReplayGameAllocationPoolUnusedFirst = NULL;
static TGMPoolAllocator* s_ReplayGameAllocationPoolUsedFirst = NULL;

TGMReplayGame* AllocateReplayGame(void)
{
	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMReplayGame), &s_ReplayGameAllocationPoolUnusedFirst, &s_ReplayGameAllocationPoolUsedFirst);
	TGMReplayGame* replayGame = (TGMReplayGame *)allocator->_object;
	replayGame->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	replayGame->_retainCount = 1;
	
	MemIncreaseGlobalCount(g_replayGamesLiving);
	
	return replayGame;
}

void DeallocateReplayGame(TGMReplayGame* replayGame)
{
	dassert(replayGame);
	if (!replayGame) return;
	
	ReleaseGame(replayGame->_game);
	
	TGMPoolAllocator* allocator = (TGMPoolAllocator*)replayGame->_holder; // need to do this before memset...
	
#ifdef DEBUG
	memset(replayGame, (int)0xDEADBEEF, sizeof(TGMReplayGame));
	replayGame->_retainCount = 0;
	replayGame->_holder = allocator;
#endif
	
	DeallocatePoolObject(allocator, &s_ReplayGameAllocationPoolUnusedFirst, &s_ReplayGameAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_replayGamesLiving);
}

TGMReplayGame* CreateReplayGame(TGMGameLog* gameLog, TGRandom* newRandom)
{
	TGMReplayGame* replayGame = AllocateReplayGame();
	replayGame->_game = CreateGame(gameLog, newRandom);
	for (PlayerColor pc = PlayerColor::Min; pc <= PlayerColor::Max; IteratePlayerColor(pc))
	{
		TGMPlayer* dumbPlayer = CreateDumbPlayer(PlayerColorToString(pc), GameLogStrategyForPlayerColor(gameLog, pc), pc);
		GameAddPlayer(replayGame->_game, dumbPlayer);
		ReleasePlayer(dumbPlayer);
	}
	
	return replayGame;
}

TGMReplayGame* RetainReplayGame(TGMReplayGame* replayGame)
{
	if (replayGame)
	{
		LogMemoryRetain("REPLAYGAME", replayGame, MemIncreaseRetainCount(replayGame->_retainCount));
	}
	
	return replayGame;
}

void ReleaseReplayGame(TGMReplayGame* replayGame)
{
	if (!replayGame) return;
	
	int32_t rc = MemDecreaseRetainCount(replayGame->_retainCount);
	LogMemoryRelease("REPLAYGAME", replayGame, rc);
	
	dassert(rc >= 0);
	if (rc == 0)
	{
		DeallocateReplayGame(replayGame);
	}
}

void ReplayGameGo(TGMReplayGame* replayGame)
{
	dassert(replayGame);
	if (!replayGame)
	{
		return;
	}
	
	GameStartNew(replayGame->_game);
}
