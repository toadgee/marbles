//
//  CardList.h
//  Marbles
//
//  Created by Todd Harris on 2/18/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#pragma once
#include "Card.h"
#include <functional>

typedef std::function<bool(unsigned i, TGMCard* card)> TGMCardListIterationBlock;

#define TGMCardList struct TGMCardListStruct

struct TGMCardListStruct
{
	int32_t _retainCount;
	
	TGMCard* _first;
	TGMCard* _last;
	unsigned _count;
	
	void *_holder;
};

TGMCardList* CreateCardList(void);
#if 0
void RetainCardList(TGMCardList* cardList);
#else
#define RetainCardList(cardList) MemIncreaseRetainCount(cardList->_retainCount)
#endif
void ReleaseCardList(TGMCardList* cardList);

TGMCardList* CopyCardList(TGMCardList* cardList);

TGMCard* CardAtIndex(TGMCardList* cardList, unsigned index);
// deprecated
void CardListIterateWithBlock(TGMCardList* cardList, TGMCardListIterationBlock block);
void ClearCardList(TGMCardList* cardList);
void CardListAdd(TGMCardList* cardList, TGMCard* card);
void CardListTransfer(TGMCardList* cardList, TGMCard* card); // like CardListAdd but doesn't retain the card
void CardListRemove(TGMCardList* cardList, TGMCard* card);
void CardListInsertBefore(TGMCardList* cardList, TGMCard* beforeCard, TGMCard* cardToInsert);
TGMCard* CardListRemoveAtIndex(TGMCardList *cardList, unsigned index);
void TransferCardFromList(TGMCard* card, TGMCardList* fromList, TGMCardList* toList);
bool AreCardListsEqual(TGMCardList* cardList1, TGMCardList* cardList2);
TGMCard* CardListRemoveCardLike(TGMCardList* cardList, TGMCard* card);
void CardListSort(TGMCardList* cardList);

#define CardListCount(cardList) cardList->_count
#define FirstCardNoRef(cardList) cardList->_first
