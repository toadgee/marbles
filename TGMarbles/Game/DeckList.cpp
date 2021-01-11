#include "precomp.h"
#include "DebugAssert.h"
#include "DeckList.h"
#include "MarblesMem.h"

TGMDeckList* CreateDeckList(void)
{
	MemIncreaseGlobalCount(g_deckListsLiving);

	TGMDeckList* deckList = static_cast<TGMDeckList*>(malloc(sizeof(TGMDeckList)));
	deckList->_retainCount = 1;
	deckList->_first = nullptr;
	deckList->_last = nullptr;
	deckList->_count = 0;
	
#ifdef DECKLIST_MEMORY_LOGGING
	int64_t dlcc = MemIncreaseGlobalCount(g_deckListCreateCount);
	NSLog(@"<<DECKLIST %p : %d (+) %lld>>", deckList, deckList->_retainCount, dlcc);
#endif
	
	return deckList;
}

TGMDeckList* RetainDeckList(TGMDeckList* deckList)
{
	if (deckList)
	{
		int32_t rc = MemIncreaseRetainCount(deckList->_retainCount);
#ifdef DECKLIST_MEMORY_LOGGING
		NSLog(@"<<DECKLIST %p : %d (+)>>", deckList, rc);
#else 
		(void)rc;
#endif
	}

	return deckList;
}
void ReleaseDeckList(TGMDeckList* deckList)
{
	if (!deckList) return;

	int32_t rc = MemDecreaseRetainCount(deckList->_retainCount);
#ifdef DECKLIST_MEMORY_LOGGING
	NSLog(@"<<DECKLIST %p : %d (-)>>", deckList, rc);
#endif

	dassert(rc >= 0);
	if (rc == 0)
	{
		DeckListIterateWithBlock(deckList, [](unsigned /*i*/, TGMDeck *deck) {
			ReleaseDeck(deck);
			return true;
		});
		
#ifdef DEBUG
		memset(deckList, static_cast<int>(0xDEADBEEF), sizeof(TGMDeckList));
		deckList->_retainCount = 0;
#endif
		MemDecreaseGlobalCount(g_deckListsLiving);
		free(deckList);
	}
}

TGMDeck* DeckListDeckAtIndex(TGMDeckList* deckList, uint16_t index, bool ownRef)
{
	TGMDeck* d = nullptr;
	unsigned i = 0;
	TGMDeck* deck = deckList->_first;
	while (deck != nullptr)
	{
		if (i == index)
		{
			d = deck;
			if (ownRef) RetainDeck(d);
			break;
		}
		
		deck = deck->_nextDeck;
		i++;
	}
	
	return d;
}

void DeckListIterateWithBlock(TGMDeckList* deckList, TGMDeckListIterationBlock block)
{
	dassert(block != nullptr);
	if (!block) return;

	unsigned i = 0;
	TGMDeck* deck = deckList->_first;
	while (deck != nullptr)
	{
		TGMDeck* next = deck->_nextDeck;
		bool goOn = block(i, deck);
		if (!goOn) 
			break;
		deck = next;
		i++;
	}
	
	return;
}

void DeckListAdd(TGMDeckList* deckList, TGMDeck* deck)
{
	if (!deck) return;

	RetainDeck(deck);
	dassert(deck->_nextDeck == nullptr);
	dassert(deck->_previousDeck == nullptr);

	if (deckList->_first == nullptr)
	{
		deckList->_first = deck;
	}
	else
	{
		deckList->_last->_nextDeck = deck;
		deck->_previousDeck = deckList->_last;
	}

	deckList->_last = deck;
	deckList->_count++;
}

TGMDeck* DeckListRemoveFirst(TGMDeckList* deckList)
{
	dassert(deckList->_first != nullptr);
	TGMDeck* deck = deckList->_first;
	deckList->_first = deck->_nextDeck;
	if (deckList->_first != nullptr)
		deckList->_first->_previousDeck = nullptr;
		
	deck->_nextDeck = nullptr;
	deck->_previousDeck = nullptr;
	deckList->_count--;
	
	return deck;
}

