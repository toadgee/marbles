//
//	Card.m
//	Marbles
//
//	Created by todd harris on 11/27/11.
//	Copyright 2012 toadgee.com. All rights reserved.
//

#include "precomp.h"
#include "Card.h"
#include "DebugAssert.h"
#include "MarblesMem.h"
#include "PoolAllocator.h"

TGMCard* AllocateCard(void);

static std::mutex s_lock;
static TGMPoolAllocator* s_CardAllocationPoolUnusedFirst = NULL;
static TGMPoolAllocator* s_CardAllocationPoolUsedFirst = NULL;

TGMCard* AllocateCard(void)
{
	const std::lock_guard<std::mutex> lock(s_lock);

	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMCard), &s_CardAllocationPoolUnusedFirst, &s_CardAllocationPoolUsedFirst);
	TGMCard* card = (TGMCard *)allocator->_object;
	card->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	MemIncreaseGlobalCount(g_cardsLiving);
	
	return card;
}

void DeallocateCard(TGMCard* card)
{
	const std::lock_guard<std::mutex> lock(s_lock);

#ifdef DEBUG
	TGMPoolAllocator* allocator = (TGMPoolAllocator*)card->_holder; // need to do this before memset...
	memset(card, (int)0xDEADBEEF, sizeof(TGMCard));
	card->_retainCount = 0;
	card->_holder = allocator;
#endif
	
	DeallocatePoolObject((TGMPoolAllocator*)card->_holder, &s_CardAllocationPoolUnusedFirst, &s_CardAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_cardsLiving);
}

TGMCard* CreateCard(int uniqueId, CardNumber number, CardSuit suit)
{
	dassert(IsCardNumber(number));
	dassert(IsCardSuit(suit));

	TGMCard* card = AllocateCard();
	dassert(card != NULL);
	
	card->nextCard = NULL;
	card->previousCard = NULL;
	card->_uniqueId = uniqueId;
	card->_number = number;
	card->_suit = suit;
	card->_retainCount = 1;
	
#ifdef CARD_MEMORY_LOGGING
	int64_t ccc = MemIncreaseGlobalCount(g_cardCreateCount);
	NSLog(@"<<CARD %p : %d (+) %lld>>", card, card->_retainCount, ccc);
#endif
	
	return card;
}

#if 0
TGMCard* CopyCard(TGMCard *card)
{
	return CreateCard(card->_uniqueId, card->_number, card->_suit);
}

void RetainCard(TGMCard* card)
{
	int32_t rc = MemIncreaseRetainCount(card->_retainCount);
		
#ifdef CARD_MEMORY_LOGGING
	NSLog(@"<<CARD %p : %d (+)>>", card, rc);
#else
#pragma unused(rc)
#endif
}

void ReleaseCard(TGMCard *card)
{
	int32_t rc = MemDecreaseRetainCount(card->_retainCount);
#ifdef CARD_MEMORY_LOGGING
	NSLog(@"<<CARD %p : %d (-)>>", card, rc);
#endif
	
	dassert(rc >= 0);
	if (rc == 0)
	{
		DeallocateCard(card);
	}
}
#endif

bool AreCardsEqual(TGMCard* card1, TGMCard* card2)
{
	if (card1 == card2) return true;
	if (card1 == NULL || card2 == NULL) return false;
	
	return (card1->_number == card2->_number)
		&& (card1->_suit == card2->_suit)
		&& (card1->_uniqueId == card2->_uniqueId);
}

bool AreCardsEquivalent(TGMCard* card1, TGMCard* card2)
{
	if (card1 == NULL && card2 == NULL) return true;
	if (card1 == NULL || card2 == NULL || (card1->_number != card2->_number))
	{
		return false;
	}
	
	return true;
}

TGMCardComparisonResult CompareCards(TGMCard* card1, TGMCard* card2)
{
	if (card1 == NULL && card2 == NULL)
	{
		return TGMCardComparisonResult::Same;
	}
	else if (card1 == NULL)
	{
		return TGMCardComparisonResult::Ascending;
	}
	else if (card2 == NULL)
	{
		return TGMCardComparisonResult::Descending;
	}
	else if (card2->_number < card1->_number)
	{
		return TGMCardComparisonResult::Ascending;
	}
	else if (card2->_number > card1->_number)
	{
		return TGMCardComparisonResult::Descending;
	}
	else if (card2->_suit < card1->_suit)
	{
		return TGMCardComparisonResult::Ascending;
	}
	else if (card2->_suit > card1->_suit)
	{
		return TGMCardComparisonResult::Descending;
	}
	else if (card2->_uniqueId < card1->_uniqueId)
	{
		return TGMCardComparisonResult::Ascending;
	}
	else if (card2->_uniqueId > card1->_uniqueId)
	{
		return TGMCardComparisonResult::Descending;
	}
	
	return TGMCardComparisonResult::Same;
}

TGMCardComparisonResult CompareCardsForDiscard(TGMCard* card1, TGMCard* card2)
{
	TGMCardComparisonResult regularCompare = CompareCards(card1, card2);
	if (regularCompare == TGMCardComparisonResult::Ascending) return TGMCardComparisonResult::Descending;
	if (regularCompare == TGMCardComparisonResult::Descending) return TGMCardComparisonResult::Ascending;
	return TGMCardComparisonResult::Same;
}

