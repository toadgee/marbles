#include "precomp.h"
#include "DebugAssert.h"
#include "MarblesMem.h"
#include "MoveList.h"
#include "Move.h"

TGMMoveList* AllocateMoveList(void);
void DeallocateMoveList(TGMMoveList* moveList);

TGMMoveList* AllocateMoveList(void)
{
	MemoryPool &pool = GetCurrentPool();
	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMMoveList), &pool.MoveListAllocationPoolUnusedFirst, &pool.MoveListAllocationPoolUsedFirst);
	TGMMoveList* moveList = static_cast<TGMMoveList *>(allocator->_object);
	moveList->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	MemIncreaseGlobalCount(g_moveListsLiving);
	
	return moveList;
}

void DeallocateMoveList(TGMMoveList* moveList)
{
	dassert(moveList);
	if (!moveList) return;
	
	{
		TGMMove* move = moveList->first;
		while (move != nullptr)
		{
			TGMMove* next = move->nextMove;
			ReleaseMove(move);
			move = next;
		}
	}
	
	TGMPoolAllocator* allocator = static_cast<TGMPoolAllocator*>(moveList->_holder); // need to do this before memset...
	
#ifdef DEBUG
	memset(moveList, static_cast<int>(0xDEADBEEF), sizeof(TGMMoveList));
	moveList->_retainCount = 0;
	moveList->_holder = allocator;
#endif
	
	MemoryPool &pool = GetCurrentPool();
	DeallocatePoolObject(allocator, &pool.MoveListAllocationPoolUnusedFirst, &pool.MoveListAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_moveListsLiving);
}

TGMMoveList* CreateMoveList(void)
{
	TGMMoveList* moveList = AllocateMoveList();
	moveList->_retainCount = 1;
	moveList->first = nullptr;
	moveList->last = nullptr;
	moveList->_count = 0;
	
#ifdef MOVELIST_MEMORY_LOGGING
	int64_t mlcc = MemIncreaseGlobalCount(g_moveListCreateCount);
	NSLog(@"<<MOVELIST %p : %d (+) %lld>>", moveList, moveList->_retainCount, mlcc);
#endif
	
	return moveList;
}

TGMMoveList* RetainMoveList(TGMMoveList* moveList)
{
	if (moveList)
	{
		int32_t rc = MemIncreaseRetainCount(moveList->_retainCount);
#ifdef MOVELIST_MEMORY_LOGGING
		NSLog(@"<<MOVELIST %p : %d (+)>>", moveList, rc);
#else 
		(void)rc;
#endif
	}

	return moveList;
}
void ReleaseMoveList(TGMMoveList* moveList)
{
	if (!moveList) return;

	int32_t rc = MemDecreaseRetainCount(moveList->_retainCount);
#ifdef MOVELIST_MEMORY_LOGGING
	NSLog(@"<<MOVELIST %p : %d (-)>>", moveList, rc);
#endif

	dassert(rc >= 0);
	if (rc == 0)
	{
		DeallocateMoveList(moveList);
	}
}

TGMMoveList* CopyMoveList(TGMMoveList* moveList)
{
	TGMMoveList* list = CreateMoveList();
	
	{
		TGMMove* move = moveList->first;
		while (move != nullptr)
		{
			TGMMove* copy = CopyMove(move);
			MoveListTransfer(list, copy);
			move = move->nextMove;
		}
	}
	
	return list;
}

void TransferMoveFromList(TGMMove* move, TGMMoveList* fromList, TGMMoveList* toList)
{
	MoveListRemoveNoRelease(fromList, move);
	MoveListTransfer(toList, move);
}

void ClearMoveList(TGMMoveList* moveList)
{
	TGMMove* move = moveList->first;
	moveList->first = nullptr;
	moveList->_count = 0;
	
	while (move != nullptr)
	{
		TGMMove* next = move->nextMove;
		
		move->nextMove = nullptr;
		move->previousMove = nullptr;
		moveList->_count--;
		ReleaseMove(move);
		
		move = next;
	}
}

/*
void MoveListConsistencyCheck(TGMMoveList *moveList)
{
	// TODO
#ifdef DEBUG
	dassert(moveList->_count >= 0);
	if (moveList->_count > 0)
	{
		dassert(moveList->first != nullptr);
		dassert(moveList->last != nullptr);
	}
	else
	{
		dassert(moveList->first == nullptr);
	}
#endif
}
*/

void MoveListAdd(TGMMoveList* moveList, TGMMove* move)
{
	RetainMove(move);
	MoveListTransfer(moveList, move);
}

void MoveListTransfer(TGMMoveList* moveList, TGMMove* move)
{
	dassert(move->nextMove == nullptr);
	dassert(move->previousMove == nullptr);

	if (moveList->first == nullptr)
	{
		moveList->first = move;
	}
	else
	{
		moveList->last->nextMove = move;
		move->previousMove = moveList->last;
	}

	moveList->last = move;
	moveList->_count++;
#ifdef DEBUG
	//MoveListConsistencyCheck();
#endif
	
}

void MoveListRemoveNoRelease(TGMMoveList* moveList, TGMMove* move)
{
	dassert(move != nullptr);
	dassert(moveList->first != nullptr);
	dassert(moveList->last != nullptr);
#ifdef DEBUG
	{
		// uh, make sure move exists in this list
		TGMMove* tmp = moveList->first;
		bool found = false;
		while (tmp != nullptr && !found)
		{
			found = (tmp == move);
			tmp = tmp->nextMove;
		}
		dassert(found);
	}
#endif
	if (move->previousMove == nullptr)
	{
		// i think we can leave moveList->last in an inconsistent state
		moveList->first = move->nextMove;
		if (moveList->first != nullptr)
		{
			moveList->first->previousMove = nullptr;
		}
	}
	else
	{
		move->previousMove->nextMove = move->nextMove;
		if (move->nextMove)
		{
			move->nextMove->previousMove = move->previousMove;
		}
		else
		{
			moveList->last = move->previousMove;
		}
	}
	
	move->nextMove = nullptr;
	move->previousMove = nullptr;
	moveList->_count--;
}

TGMMove* MoveListRemoveFirst(TGMMoveList* moveList)
{
	TGMMove* first = moveList->first;
	if (first)
	{
		MoveListRemoveNoRelease(moveList, first);
	}
	
	return first;
}

bool AreMoveListsEqual(TGMMoveList* moveList1, TGMMoveList* moveList2)
{
	if (moveList1 == moveList2)
	{
		return true;
	}
	else if (moveList1 != nullptr && moveList2 == nullptr)
	{
		return false;
	}
	else if (moveList1 == nullptr && moveList2 != nullptr)
	{
		return false;
	}
	else if (moveList1 == nullptr && moveList2 == nullptr)
	{
		// stupid branch for analyzer
		return true;
	}
	else if (moveList1->_count != moveList2->_count)
	{
		return false;
	}
	
	TGMMove *thisMove = moveList1->first;
	TGMMove *thatMove = moveList2->first;
	while (thisMove != nullptr)
	{
		if (!AreMovesEqual(thisMove, thatMove))
		{
			return false;
		}
		thisMove = thisMove->nextMove;
		thatMove = thatMove->nextMove;
	}
	
	return true;
}

