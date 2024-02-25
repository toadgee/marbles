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
	TGMCard* _first;
	TGMCard* _last;
	void *_holder;
	int32_t _retainCount;
	unsigned _count;
};

TGMCardList* CreateCardList(void);
#define RetainCardList(cardList) MemIncreaseRetainCount(cardList->_retainCount)
#define ReleaseCardList(cardList) if ((MemDecreaseRetainCount(cardList->_retainCount)) == 0) DeallocateCardList(cardList)
void DeallocateCardList(TGMCardList* cardList);

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
