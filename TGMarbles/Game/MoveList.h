//
//  MoveList.h
//  Marbles
//
//  Created by Todd on 11/8/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#pragma once
#include "Move.h"

typedef std::function<void(int i, TGMMove* move)> TGMMoveListIterationBlock;

#define TGMMoveList struct TGMMoveListStruct

struct TGMMoveListStruct
{
	int _retainCount;
	
	TGMMove* first;
	TGMMove* last;
	unsigned _count;
	
	void* _holder;
};

TGMMoveList* CreateMoveList(void);
TGMMoveList* RetainMoveList(TGMMoveList* moveList);
void ReleaseMoveList(TGMMoveList* moveList);

TGMMoveList* CopyMoveList(TGMMoveList* moveList);

/* MoveListIterateWithBlock is deprecated. Recommended:
// OldMoveListIterateWithBlock
{
	TGMMove* move = moveList->first;
	while (move != NULL)
	{
		TGMMove* next = move->nextMove;
		
		## BLOCK ##
		
		move = next;
	}
}
*/
void MoveListIterateWithBlock(TGMMoveList* moveList, TGMMoveListIterationBlock block);
void ClearMoveList(TGMMoveList* moveList);
void MoveListAdd(TGMMoveList* moveList, TGMMove* move);
void MoveListTransfer(TGMMoveList* moveList, TGMMove* move); // like MoveListAdd, but does not retain the move
void MoveListRemoveNoRelease(TGMMoveList* moveList, TGMMove* move);
TGMMove* MoveListRemoveFirst(TGMMoveList* moveList);
void TransferMoveFromList(TGMMove* move, TGMMoveList* fromList, TGMMoveList* toList);
bool AreMoveListsEqual(TGMMoveList* moveList1, TGMMoveList* moveList2);

inline void MoveListRemove(TGMMoveList* moveList, TGMMove* move)
{
	MoveListRemoveNoRelease(moveList, move);
	ReleaseMove(move);
}

#define MoveListCount(moveList) moveList->_count
