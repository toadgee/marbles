#include "precomp.h"
#include "DebugAssert.h"
#include "MarblesMem.h"
#include "MoveList.h"
#include "Move.h"

TGMMoveList* AllocateMoveList(void);
void DeallocateMoveList(TGMMoveList* moveList);

static std::mutex s_lock;
static TGMPoolAllocator* s_MoveListAllocationPoolUnusedFirst = NULL;
static TGMPoolAllocator* s_MoveListAllocationPoolUsedFirst = NULL;

TGMMoveList* AllocateMoveList(void)
{
	const std::lock_guard<std::mutex> lock(s_lock);
	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMMoveList), &s_MoveListAllocationPoolUnusedFirst, &s_MoveListAllocationPoolUsedFirst);
	TGMMoveList* moveList = (TGMMoveList *)allocator->_object;
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
		while (move != NULL)
		{
			TGMMove* next = move->nextMove;
			ReleaseMove(move);
			move = next;
		}
	}
	
	TGMPoolAllocator* allocator = (TGMPoolAllocator*)moveList->_holder; // need to do this before memset...
	
#ifdef DEBUG
	memset(moveList, (int)0xDEADBEEF, sizeof(TGMMoveList));
	moveList->_retainCount = 0;
	moveList->_holder = allocator;
#endif
	
	const std::lock_guard<std::mutex> lock(s_lock);
	DeallocatePoolObject(allocator, &s_MoveListAllocationPoolUnusedFirst, &s_MoveListAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_moveListsLiving);
}

TGMMoveList* CreateMoveList(void)
{
	TGMMoveList* moveList = AllocateMoveList();
	moveList->_retainCount = 1;
	moveList->first = NULL;
	moveList->last = NULL;
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
		while (move != NULL)
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
	moveList->first = NULL;
	moveList->_count = 0;
	
	while (move != NULL)
	{
		TGMMove* next = move->nextMove;
		
		move->nextMove = NULL;
		move->previousMove = NULL;
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
		dassert(moveList->first != NULL);
		dassert(moveList->last != NULL);
	}
	else
	{
		dassert(moveList->first == NULL);
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
	dassert(move->nextMove == NULL);
	dassert(move->previousMove == NULL);

	if (moveList->first == NULL)
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
	dassert(move != NULL);
	dassert(moveList->first != NULL);
	dassert(moveList->last != NULL);
#ifdef DEBUG
	{
		// uh, make sure move exists in this list
		TGMMove* tmp = moveList->first;
		bool found = false;
		while (tmp != NULL && !found)
		{
			found = (tmp == move);
			tmp = tmp->nextMove;
		}
		dassert(found);
	}
#endif
	if (move->previousMove == NULL)
	{
		// i think we can leave moveList->last in an inconsistent state
		moveList->first = move->nextMove;
		if (moveList->first != NULL)
		{
			moveList->first->previousMove = NULL;
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
	
	move->nextMove = NULL;
	move->previousMove = NULL;
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
	else if (moveList1 != NULL && moveList2 == NULL)
	{
		return false;
	}
	else if (moveList1 == NULL && moveList2 != NULL)
	{
		return false;
	}
	else if (moveList1 == NULL && moveList2 == NULL)
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
	while (thisMove != NULL)
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

