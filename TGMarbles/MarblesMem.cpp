#include "precomp.h"
#include "MarblesMem.h"

#ifdef DEBUG
#include <stdio.h>
#include <assert.h>

int64_t g_cardCreateCount = 0;
int64_t g_moveCreateCount = 0;
int64_t g_marbleCreateCount = 0;
int64_t g_moveListCreateCount = 0;
int64_t g_cardListCreateCount = 0;
int64_t g_cardListHolderCreateCount = 0;
int64_t g_boardCreateCount = 0;
int64_t g_deckCreateCount = 0;
int64_t g_deckListCreateCount = 0;
int64_t g_gameLogCreateCount = 0;
int64_t g_teammatesCreateCount = 0;
int64_t g_replayGamesCreateCount = 0;
int64_t g_gamesCreateCount = 0;
int64_t g_playersCreateCount = 0;

int64_t g_boardsLiving = 0;
int64_t g_cardListsLiving = 0;
int64_t g_cardListHoldersLiving = 0;
int64_t g_cardsLiving = 0;
int64_t g_decksLiving = 0;
int64_t g_deckListsLiving = 0;
int64_t g_marblesLiving = 0;
int64_t g_moveListsLiving = 0;
int64_t g_movesLiving = 0;
int64_t g_gameLogsLiving = 0;
int64_t g_teammatesLiving = 0;
int64_t g_replayGamesLiving = 0;
int64_t g_gamesLiving = 0;
int64_t g_playersLiving = 0;

MemorySnapshot CreateMemorySnapshot(void)
{
	MemorySnapshot snapshot;
	snapshot._cardListsLiving = g_cardListsLiving;
	snapshot._cardListHoldersLiving = g_cardListHoldersLiving;
	snapshot._moveListsLiving = g_moveListsLiving;
	snapshot._movesLiving = g_movesLiving;
	snapshot._marblesLiving = g_marblesLiving;
	snapshot._cardsLiving = g_cardsLiving;
	snapshot._boardsLiving = g_boardsLiving;
	snapshot._decksLiving = g_decksLiving;
	snapshot._deckListsLiving = g_deckListsLiving;
	snapshot._gameLogsLiving = g_gameLogsLiving;
	snapshot._teammatesLiving = g_teammatesLiving;
	snapshot._replayGamesLiving = g_replayGamesLiving;
	snapshot._gamesLiving = g_gamesLiving;
	snapshot._playersLiving = g_playersLiving;
	return snapshot;
}

#define MemoryCheckInternal(leaks, typeName, expected, actual) \
	if (expected != actual) { \
		fprintf(stderr, "%s living doesn't match : Original %lld != Final %lld\n", typeName, expected, actual); \
		leaks = 1; \
	}

void MemoryCheck(MemorySnapshot snapshot)
{
	MemorySnapshot now = CreateMemorySnapshot();
	uint64_t leaks = 0;
	MemoryCheckInternal(leaks, "CardList", snapshot._cardListsLiving, now._cardListsLiving);
	MemoryCheckInternal(leaks, "CardListHolder", snapshot._cardListHoldersLiving, now._cardListHoldersLiving);
	MemoryCheckInternal(leaks, "MoveList", snapshot._moveListsLiving, now._moveListsLiving);
	MemoryCheckInternal(leaks, "Move", snapshot._movesLiving, now._movesLiving);
	MemoryCheckInternal(leaks, "Marble", snapshot._marblesLiving, now._marblesLiving);
	MemoryCheckInternal(leaks, "Card", snapshot._cardsLiving, now._cardsLiving);
	MemoryCheckInternal(leaks, "Board", snapshot._boardsLiving, now._boardsLiving);
	MemoryCheckInternal(leaks, "Deck", snapshot._decksLiving, now._decksLiving);
	MemoryCheckInternal(leaks, "DeckList", snapshot._deckListsLiving, now._deckListsLiving);
	MemoryCheckInternal(leaks, "GameLog", snapshot._gameLogsLiving, now._gameLogsLiving);
	MemoryCheckInternal(leaks, "Teammates", snapshot._teammatesLiving, now._teammatesLiving);
	MemoryCheckInternal(leaks, "ReplayGames", snapshot._replayGamesLiving, now._replayGamesLiving);
	MemoryCheckInternal(leaks, "Games", snapshot._gamesLiving, now._gamesLiving);
	MemoryCheckInternal(leaks, "Players", snapshot._playersLiving, now._playersLiving);
	
	// do this last so we get a list of everything that we leaked (as opposed to figuring out which hierarchy order is the best)
	if (leaks != 0)
	{
		//NSLog(@"Failed mem test");
		assert(leaks == 0);
	}
}
#endif

