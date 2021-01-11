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
static TGMPoolAllocator* s_CardAllocationPoolUnusedFirst = nullptr;
static TGMPoolAllocator* s_CardAllocationPoolUsedFirst = nullptr;

TGMCard* AllocateCard(void)
{
	const std::lock_guard<std::mutex> lock(s_lock);

	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMCard), &s_CardAllocationPoolUnusedFirst, &s_CardAllocationPoolUsedFirst);
	TGMCard* card = static_cast<TGMCard *>(allocator->_object);
	card->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	MemIncreaseGlobalCount(g_cardsLiving);
	
	return card;
}

void DeallocateCard(TGMCard* card)
{
	const std::lock_guard<std::mutex> lock(s_lock);

#ifdef DEBUG
	TGMPoolAllocator* allocator = static_cast<TGMPoolAllocator*>(card->_holder); // need to do this before memset...
	memset(card, static_cast<int>(0xDEADBEEF), sizeof(TGMCard));
	card->_retainCount = 0;
	card->_holder = allocator;
#endif
	
	DeallocatePoolObject(static_cast<TGMPoolAllocator*>(card->_holder), &s_CardAllocationPoolUnusedFirst, &s_CardAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_cardsLiving);
}

TGMCard* CreateCard(int uniqueId, CardNumber number, CardSuit suit)
{
	dassert(IsCardNumber(number));
	dassert(IsCardSuit(suit));

	TGMCard* card = AllocateCard();
	dassert(card != nullptr);
	
	card->nextCard = nullptr;
	card->previousCard = nullptr;
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
	if (card1 == nullptr || card2 == nullptr) return false;
	
	return (card1->_number == card2->_number)
		&& (card1->_suit == card2->_suit)
		&& (card1->_uniqueId == card2->_uniqueId);
}

bool AreCardsEquivalent(TGMCard* card1, TGMCard* card2)
{
	if (card1 == nullptr && card2 == nullptr) return true;
	if (card1 == nullptr || card2 == nullptr || (card1->_number != card2->_number))
	{
		return false;
	}
	
	return true;
}

TGMCardComparisonResult CompareCards(TGMCard* card1, TGMCard* card2)
{
	if (card1 == nullptr && card2 == nullptr)
	{
		return TGMCardComparisonResult::Same;
	}
	else if (card1 == nullptr)
	{
		return TGMCardComparisonResult::Ascending;
	}
	else if (card2 == nullptr)
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

