#include "precomp.h"
#include "DataExtensions.h"
#include "DebugAssert.h"
#include "Deck.h"
#include "GameLog.h"
#include "MarblesMem.h"
#include "Move.h"
#include "MoveList.h"

TGMGameLog* CreateGameLog(void)
{
	MemIncreaseGlobalCount(g_gameLogsLiving);
	
	TGMGameLog* gameLog = static_cast<TGMGameLog*>(malloc(sizeof(TGMGameLog)));
	gameLog->_dealingPlayer = PlayerColor::None;
	gameLog->_deckList = CreateDeckList();
	gameLog->_moveList = CreateMoveList();
	gameLog->_retainCount = 1;
	for (unsigned p = 0; p < kPlayers; p++)
	{
		gameLog->_playerStrategy[p] = Strategy::None;
	}
	
#ifdef CARD_MEMORY_LOGGING
	int64_t glcc = MemIncreaseGlobalCount(g_gameLogCreateCount);
	NSLog(@"<<GAMELOG %p : %d (+) %lld>>", gameLog, gameLog->_retainCount, glcc);
#endif
	
	return gameLog;
}

void RetainGameLog(TGMGameLog* gameLog)
{
	int32_t rc = MemIncreaseRetainCount(gameLog->_retainCount);
		
#ifdef GAMELOG_MEMORY_LOGGING
	NSLog(@"<<GAMELOG %p : %d (+)>>", gameLog, rc);
#else
	(void)rc;
#endif
}

TGMGameLog* CopyGameLog(TGMGameLog* gameLog)
{
	TGMGameLog* copy = nullptr;
	if (gameLog)
	{
		TGMData data = GameLogData(gameLog);
		copy = CreateGameLogFromData(data);
	}
	
	return copy;
}

void ReleaseGameLog(TGMGameLog* gameLog)
{
	if (!gameLog) return;
	
	int32_t rc = MemDecreaseRetainCount(gameLog->_retainCount);
#ifdef GAMELOG_MEMORY_LOGGING
	NSLog(@"<<GAMELOG %p : %d (-)>>", gameLog, rc);
#endif
	
	dassert(rc >= 0);
	if (rc == 0)
	{
		ReleaseMoveList(gameLog->_moveList);
		ReleaseDeckList(gameLog->_deckList);
		
#ifdef DEBUG
		memset(gameLog, static_cast<int>(0xDEADBEEF), sizeof(TGMGameLog));
		gameLog->_retainCount = 0;
#endif
		MemDecreaseGlobalCount(g_gameLogsLiving);
		free(gameLog);
	}
}

void GameLogSetMoveList(TGMGameLog* gameLog, TGMMoveList* moveList)
{
	ReleaseMoveList(gameLog->_moveList);
	RetainMoveList(moveList);
	gameLog->_moveList = moveList;
}

Strategy GameLogStrategyForPlayerColor(TGMGameLog* gameLog, PlayerColor pc)
{
	dassert(IsPlayerColor(pc));
	return gameLog->_playerStrategy[PositionForPlayerColor(pc)];
}

void GameLogSetStrategyForPlayerColor(TGMGameLog* gameLog, Strategy strategy, PlayerColor pc)
{
	dassert(IsPlayerColor(pc));
	dassert(IsValidStrategy(strategy));
	
	gameLog->_playerStrategy[PositionForPlayerColor(pc)] = strategy;
}

void GameLogAddDeck(TGMGameLog* gameLog, TGMDeck* deck)
{
	TGMDeck* copy = CopyDeck(deck);
	DeckListAdd(gameLog->_deckList, copy);
	ReleaseDeck(copy);
}

void GameLogAddMove(TGMGameLog* gameLog, TGMMove* move)
{
	TGMMove* copy = CopyMove(move);
	MoveListTransfer(gameLog->_moveList, copy);
}

TGMDeck* GameLogPopDeck(TGMGameLog* gameLog)
{
	if (!GameLogHasDeck(gameLog))
	{
		dassert(false);
		return nullptr;
	}
	
	return DeckListRemoveFirst(gameLog->_deckList);
}

bool GameLogHasDeck(TGMGameLog* gameLog)
{
	bool hasDeck = false;
	if (gameLog->_deckList != nullptr)
	{
		hasDeck = (DeckListCount(gameLog->_deckList) > 0);
	}
	
	return hasDeck;
}

TGMMove* GameLogPopMove(TGMGameLog* gameLog)
{
	return MoveListRemoveFirst(gameLog->_moveList);
}

bool GameLogHasMove(TGMGameLog* gameLog)
{
	return (MoveListCount(gameLog->_moveList) > 0);
}

bool AreGameLogsEqual(TGMGameLog* gameLog1, TGMGameLog* gameLog2)
{
	if (gameLog1 == gameLog2) return true;
	if (gameLog1 == nullptr && gameLog2 != nullptr) return false;
	if (gameLog1 != nullptr && gameLog2 == nullptr) return false;
	if (gameLog1 == nullptr || gameLog2 == nullptr) return false; // stupid analyzer branch
	
	if (gameLog1->_dealingPlayer != gameLog2->_dealingPlayer)
	{
		return false;
	}
	
	if (DeckListCount(gameLog1->_deckList) != DeckListCount(gameLog2->_deckList))
	{
		return false;
	}
	
	// TODO : add are deck lists equal to decklist!
	for (uint16_t dc = 0; dc < DeckListCount(gameLog1->_deckList); dc++)
	{
		TGMDeck* thisDeck = DeckListDeckAtIndex(gameLog1->_deckList, dc, false);
		TGMDeck* thatDeck = DeckListDeckAtIndex(gameLog2->_deckList, dc, false);
		
		if (!AreDecksEqual(thisDeck, thatDeck))
		{
			return false;
		}
	}
	
	if (!AreMoveListsEqual(gameLog1->_moveList, gameLog2->_moveList))
	{
		return false;
	}
	
	return true;
}
