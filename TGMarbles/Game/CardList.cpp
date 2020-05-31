//
//  CardList.m
//  Marbles
//
//  Created by Todd Harris on 2/18/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#include "precomp.h"
#include "CardList.h"
#include "Card.h"
#include "DebugAssert.h"
#include "MarblesMem.h"

TGMCardList* AllocateCardList(void);
void DeallocateCardList(TGMCardList* cardList);

static std::mutex s_lock;
static TGMPoolAllocator* s_CardListAllocationPoolUnusedFirst = NULL;
static TGMPoolAllocator* s_CardListAllocationPoolUsedFirst = NULL;

TGMCardList* AllocateCardList(void)
{
	const std::lock_guard<std::mutex> lock(s_lock);

	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMCardList), &s_CardListAllocationPoolUnusedFirst, &s_CardListAllocationPoolUsedFirst);
	TGMCardList* cardList = (TGMCardList *)allocator->_object;
	cardList->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	MemIncreaseGlobalCount(g_cardListsLiving);
	
	return cardList;
}

void DeallocateCardList(TGMCardList* cardList)
{
	const std::lock_guard<std::mutex> lock(s_lock);

	dassert(cardList);
	if (!cardList) return;
	
	TGMCard* card = FirstCardNoRef(cardList);
	while (card != NULL)
	{
		TGMCard* next = card->nextCard;
		ReleaseCard(card);
		card = next;
	}
	
	TGMPoolAllocator* allocator = (TGMPoolAllocator*)cardList->_holder; // need to do this before memset...
#ifdef DEBUG
	memset(cardList, (int)0xDEADBEEF, sizeof(TGMCardList));
	cardList->_retainCount = 0;
	cardList->_holder = allocator;
#endif
	
	DeallocatePoolObject(allocator, &s_CardListAllocationPoolUnusedFirst, &s_CardListAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_cardListsLiving);
}


TGMCardList* CreateCardList(void)
{
	TGMCardList* cardList = AllocateCardList();
	cardList->_retainCount = 1;
	cardList->_first = NULL;
	cardList->_last = NULL;
	cardList->_count = 0;
	
#ifdef CARDLIST_MEMORY_LOGGING
	int64_t mlcc = MemIncreaseGlobalCount(g_cardListCreateCount);
	NSLog(@"<<CARDLIST %p : %d (+) %lld>>", cardList, cardList->_retainCount, mlcc);
#endif
	
	return cardList;
}

#if 0
void RetainCardList(TGMCardList* cardList)
{
	int32_t rc = MemIncreaseRetainCount(cardList->_retainCount);
#ifdef CARDLIST_MEMORY_LOGGING
	NSLog(@"<<CARDLIST %p : %d (+)>>", cardList, rc);
#else 
#pragma unused(rc)
#endif
}
#endif

void ReleaseCardList(TGMCardList* cardList)
{
	int32_t rc = MemDecreaseRetainCount(cardList->_retainCount);
#ifdef CARDLIST_MEMORY_LOGGING
	NSLog(@"<<CARDLIST %p : %d (-)>>", cardList, rc);
#endif

	dassert(rc >= 0);
	if (rc == 0)
	{
		DeallocateCardList(cardList);
	}
}

TGMCardList* CopyCardList(TGMCardList* cardList)
{
	TGMCardList* list = CreateCardList();
	TGMCard* card = FirstCardNoRef(cardList);
	while (card != NULL)
	{
		TGMCard* copy = CopyCard(card);
		CardListTransfer(list, copy);
		card = card->nextCard;
	}
	
	return list;
}

void TransferCardFromList(TGMCard* card, TGMCardList* fromList, TGMCardList* toList)
{
	RetainCard(card);
	CardListRemove(fromList, card);
	CardListTransfer(toList, card);
}

TGMCard* CardAtIndex(TGMCardList* cardList, unsigned index)
{
	TGMCard* c = NULL;
	unsigned i = 0;
	TGMCard* card = cardList->_first;
	while (card != NULL)
	{
		if (i == index)
		{
			c = card;
			RetainCard(card);
			break;
		}
		
		card = card->nextCard;
		i++;
	}
	
	return c;
}
void CardListIterateWithBlock(TGMCardList* cardList, TGMCardListIterationBlock block)
{
	dassert(block != NULL);
	if (!block) return;

	unsigned i = 0;
	TGMCard* card = cardList->_first;
	while (card != NULL)
	{
		TGMCard* next = card->nextCard;
		bool goOn = block(i, card);
		if (!goOn) 
			break;
		card = next;
		i++;
	}
	
	return;
}

void ClearCardList(TGMCardList* cardList)
{
	TGMCard* card = cardList->_first;
	cardList->_first = NULL;
	cardList->_count = 0;
	
	while (card != NULL)
	{
		TGMCard* next = card->nextCard;
		
		card->nextCard = NULL;
		card->previousCard = NULL;
		cardList->_count--;
		ReleaseCard(card);
		
		card = next;
	}
}

void CardListAdd(TGMCardList* cardList, TGMCard* card)
{
	if (!card) return;
	RetainCard(card);
	CardListTransfer(cardList, card);
}

void CardListTransfer(TGMCardList* cardList, TGMCard* card)
{
	if (!card) return;
	
	dassert(card->nextCard == NULL);
	dassert(card->previousCard == NULL);

	if (cardList->_first == NULL)
	{
		cardList->_first = card;
	}
	else
	{
		cardList->_last->nextCard = card;
		card->previousCard = cardList->_last;
	}

	cardList->_last = card;
	cardList->_count++;
}

void CardListRemove(TGMCardList* cardList, TGMCard* card)
{
	dassert(card != NULL);
	dassert(cardList->_first != NULL);
	dassert(cardList->_last != NULL);
#ifdef DEBUG
	{
		// uh, make sure card exists in this list
		TGMCard* tmp = cardList->_first;
		bool found = false;
		while (tmp != NULL && !found)
		{
			found = (tmp == card);
			tmp = tmp->nextCard;
		}
		dassert(found);
	}
#endif
	if (card == NULL) return;
	
	if (card->previousCard == NULL)
	{
		// i think we can leave cardList->_last in an inconsistent state
		cardList->_first = card->nextCard;
		if (cardList->_first != NULL)
			cardList->_first->previousCard = NULL;
	}
	else
	{
		card->previousCard->nextCard = card->nextCard;
		if (card->nextCard)
		{
			card->nextCard->previousCard = card->previousCard;
		}
		else
		{
			cardList->_last = card->previousCard;
		}
	}
	
	card->nextCard = NULL;
	card->previousCard = NULL;
	cardList->_count--;
	ReleaseCard(card);
	
#ifdef DEBUG
	// TODO consistency check through the logic in self.count
#endif
}

TGMCard* CardListRemoveAtIndex(TGMCardList *cardList, unsigned index)
{
	TGMCard* card = cardList->_first;
	for (unsigned i = 0; i < index; i++)
	{
		card = card->nextCard;
	}
	
	RetainCard(card);
	CardListRemove(cardList, card);
	return card;
}

void CardListInsertBefore(TGMCardList* cardList, TGMCard* beforeCard, TGMCard* cardToInsert)
{
	dassert(cardToInsert != NULL);
	dassert(cardToInsert->nextCard == NULL);
	dassert(cardToInsert->previousCard == NULL);
	
	if (cardToInsert == NULL) return;
	
	RetainCard(cardToInsert);
	if (cardList->_first == NULL || (beforeCard != NULL && beforeCard->previousCard == NULL))
	{
		// we're at first
		cardToInsert->nextCard = cardList->_first;
		if (cardList->_first != NULL)
			cardList->_first->previousCard = cardToInsert;
		if (cardList->_first == NULL)
		{
			cardList->_last = cardToInsert;
		}
		cardList->_first = cardToInsert;
	}
	else if (beforeCard == NULL)
	{
		// we're at last
		cardToInsert->previousCard = cardList->_last;
		cardList->_last->nextCard = cardToInsert;
		cardList->_last = cardToInsert;
	}
	else
	{
		// we're somewhere in the middle
		cardToInsert->previousCard = beforeCard->previousCard;
		cardToInsert->previousCard->nextCard = cardToInsert;
		cardToInsert->nextCard = beforeCard;
		beforeCard->previousCard = cardToInsert;
	}
	
	cardList->_count++;
}

bool AreCardListsEqual(TGMCardList* cardList1, TGMCardList* cardList2)
{
	if (cardList1 == cardList2)
	{
		return true;
	}
	else if (cardList1 != NULL && cardList2 == NULL)
	{
		return false;
	}
	else if (cardList1 == NULL && cardList2 != NULL)
	{
		return false;
	}
	else if (cardList1 == NULL || cardList2 == NULL)
	{
		// stupid branch for analyzer
		return true;
	}
	else if (cardList1->_count != cardList2->_count)
	{
		return false;
	}
	
	TGMCard *thisCard = cardList1->_first;
	TGMCard *thatCard = cardList2->_first;
	while (thisCard != NULL)
	{
		if (!AreCardsEqual(thisCard, thatCard))
		{
			return false;
		}
		thisCard = thisCard->nextCard;
		thatCard = thatCard->nextCard;
	}
	
	return true;
}

TGMCard* CardListRemoveCardLike(TGMCardList* cardList, TGMCard* card)
{
	dassert(card != NULL);
	
	TGMCard* listCard = NULL;
	
	TGMCard* c = FirstCardNoRef(cardList);
	while (c != NULL)
	{
		if (AreCardsEqual(c, card))
		{
			listCard = c;
			RetainCard(c);
			break;
		}
		
		c = c->nextCard;
	}
	
	dassert(listCard != NULL);
	CardListRemove(cardList, listCard);
	return listCard;
}

void CardListSort(TGMCardList* cardList)
{
#ifdef DEBUG
	unsigned cc = CardListCount(cardList);
#endif

	TGMCardList* sorted = CreateCardList();
	
	TGMCard *card = FirstCardNoRef(cardList);
	while (card != NULL)
	{
		TGMCard* next = card->nextCard;
		TGMCard* beforeCard = NULL;
		
		TGMCard *handCard = FirstCardNoRef(sorted);
		while (handCard != NULL)
		{
			if (CompareCards(card, handCard) == TGMCardComparisonResult::Descending)
			{
				beforeCard = handCard;
				break;
			}
			
			handCard = handCard->nextCard;
		}
		
		RetainCard(card);
		//NSLog(@"BEFORE %@ : %@ %d", CardDescription(card, false), CardListDescription(sorted, NO), sorted->count);
		CardListRemove(cardList, card);
		CardListInsertBefore(sorted, beforeCard, card);
		//NSLog(@"AFTER %@ : %@ %d", CardDescription(card, false), CardListDescription(sorted, NO), sorted->count);
		ReleaseCard(card);
		
		card = next;
	}

	dassert(cc == CardListCount(sorted));
	dassert(CardListCount(cardList) == 0);
	
	// now we need to move cardList internals
	cardList->_first = sorted->_first;
	cardList->_last = sorted->_last;
	cardList->_count = sorted->_count;
	sorted->_first = NULL;
	sorted->_last = NULL;
	sorted->_count = 0;
	
	// sorted is now done
	ReleaseCardList(sorted);
	
	return;
}
