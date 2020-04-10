#include "precomp.h"
#include "Card.h"
#include "Deck.h"
#include "DebugAssert.h"
#include "MarblesMem.h"

#define kNumberOfJokersPerDeck	2 // number of jokers in the deck

uint16_t DeckCapacity()
{
	// normally we'd add 1 to suit & card diffs, but since we're excluding jokers from both it works out
	uint16_t retval = kDecksToShuffle * ((CardSuit_Max - CardSuit_Min) * (CardNumber_Max - CardNumber_Min) + kNumberOfJokersPerDeck);
	dassert(retval == 108);
	return retval;
}


TGMDeck* CreateNonEmptyDeck(void)
{
	return CreateDeck(false);
}

TGMDeck* CreateDeck(bool emptyDeck)
{
	MemIncreaseGlobalCount(g_decksLiving);

	TGMDeck* deck = (TGMDeck*)malloc(sizeof(TGMDeck));
	deck->_retainCount = 1;
	deck->_cards = CreateCardList();
	deck->_discarded = CreateCardList();
	deck->_nextDeck = NULL;
	deck->_previousDeck = NULL;
	
	if (!emptyDeck)
	{
		int uniqueId = 0;
		for (int decks = 0; decks < kDecksToShuffle; decks++)
		{
			// we only want 2 jokers, so don't include them here
			for (CardSuit suit = CardSuit_Min; suit <= CardSuit_Max; IterateCardSuit(suit))
			{
				if (suit == CardSuit_Joker) continue;
				for (CardNumber number = CardNumber_Min; number <= CardNumber_Max; IterateCardNumber(number))
				{
					if (number == CardNumber_Joker) continue;
					TGMCard* card = CreateCard(uniqueId++, number, suit);
					CardListTransfer(deck->_cards, card);
				}
			}
			// just make jokers the 
			for (int jokers = 0; jokers < kNumberOfJokersPerDeck; jokers++)
			{
				TGMCard* card = CreateCard(uniqueId++, CardNumber_Joker, CardSuit_Joker);
				CardListTransfer(deck->_cards, card);
			}
		}
		
		dassert (CardListCount(deck->_cards) == DeckCapacity());
	}
	
	LogMemoryCreate("DECK", deck, MemIncreaseGlobalCount(g_deckCreateCount));
	return deck;
}

TGMDeck* CopyDeck(TGMDeck *deck)
{
	TGMDeck* deckCopy = CreateDeck(true);
	TGMCard *card = FirstCardNoRef(deck->_cards);
	while (card != NULL)
	{
		TGMCard* copy = CopyCard(card);
		CardListTransfer(deckCopy->_cards, copy);
		card = card->nextCard;
	}
	
	card = FirstCardNoRef(deck->_discarded);
	while (card != NULL)
	{
		TGMCard* copy = CopyCard(card);
		CardListTransfer(deckCopy->_discarded, copy);
		card = card->nextCard;
	}
	
	return deckCopy;
}

#if 0
void RetainDeck(TGMDeck* deck)
{
	LogMemoryRetain("DECK", deck, MemIncreaseRetainCount(deck->_retainCount));
}
#endif

void ReleaseDeck(TGMDeck *deck)
{
	assert(deck);
	
	int32_t rc = MemDecreaseRetainCount(deck->_retainCount);
	LogMemoryRelease("DECK", deck, rc);
		
	dassert(rc >= 0);
	if (rc == 0)
	{
		ReleaseCardList(deck->_discarded);
		ReleaseCardList(deck->_cards);
		
#ifdef DEBUG
		memset(deck, (int)0xDEADBEEF, sizeof(TGMDeck));
		deck->_retainCount = 0;
#endif
		MemDecreaseGlobalCount(g_decksLiving);
		free(deck);
	}
}

void DeckShuffle(TGMDeck* deck, TGRandom* rng)
{
	TGMCard *card = FirstCardNoRef(deck->_discarded);
	while (card != NULL)
	{
		TGMCard *next = card->nextCard;
		TransferCardFromList(card, deck->_discarded, deck->_cards);
		card = next;
	}
	
	unsigned count = CardListCount(deck->_cards);
	dassert(count == DeckCapacity());
	
	while (count > 0)
	{
		unsigned cardPos = RandomRandom(rng) % count; // Shuffling
		
		TGMCard* c = CardListRemoveAtIndex(deck->_cards, cardPos);
		CardListTransfer(deck->_cards, c);
		
		count--;
	}
}

TGMCard* DeckRemoveTopCard(TGMDeck* deck)
{
	return CardListRemoveAtIndex(deck->_cards, 0);
}

bool DeckIsEmpty(TGMDeck* deck)
{
	return CardListCount(deck->_cards) == 0;
}

void DeckClear(TGMDeck* deck)
{
	TGMCard *card = FirstCardNoRef(deck->_discarded);
	while (card != NULL)
	{
		TGMCard *next = card->nextCard;
		TransferCardFromList(card, deck->_discarded, deck->_cards);
		card = next;
	}
}

void DeckAddDiscard(TGMDeck* deck, TGMCard* discard)
{
	CardListAdd(deck->_discarded, discard);
}

bool AreDecksEqual(TGMDeck* deck1, TGMDeck* deck2)
{
	if (deck1 == deck2) return true;
	if (deck1 != NULL && deck2 == NULL) return false;
	if (deck2 == NULL && deck2 != NULL) return false;
	// stupid branch for analyzer
	if (deck1 == NULL || deck2 == NULL) return true;
	
	if (CardListCount(deck1->_cards) != CardListCount(deck2->_cards)) return false;
	if (CardListCount(deck1->_discarded) != CardListCount(deck2->_discarded)) return false;
	
	TGMCard* thisCard;
	TGMCard* thatCard;
	
	thisCard = FirstCardNoRef(deck1->_cards);
	thatCard = FirstCardNoRef(deck2->_cards);
	while (thisCard != NULL)
	{
		if (!AreCardsEqual(thisCard, thatCard)) return false;
		thisCard = thisCard->nextCard;
		thatCard = thatCard->nextCard;
	}
	
	thisCard = deck1->_discarded->_first;
	thatCard = deck2->_discarded->_first;
	while (thisCard != NULL)
	{
		if (!AreCardsEqual(thisCard, thatCard)) return false;
		thisCard = thisCard->nextCard;
		thatCard = thatCard->nextCard;
	}
	
	return true;
}

