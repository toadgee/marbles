#include "precomp.h"
#include "Card.h"
#include "Deck.h"
#include "DebugAssert.h"
#include "MarblesMem.h"

#define kNumberOfJokersPerDeck	2 // number of jokers in the deck

uint16_t DeckCapacity()
{
	// normally we'd add 1 to suit & card diffs, but since we're excluding jokers from both it works out
	uint16_t retval = kDecksToShuffle * ((static_cast<int>(CardSuit::Max) - static_cast<int>(CardSuit::Min)) * (static_cast<int>(CardNumber::Max) - static_cast<int>(CardNumber::Min)) + kNumberOfJokersPerDeck);
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

	TGMDeck* deck = static_cast<TGMDeck*>(malloc(sizeof(TGMDeck)));
	deck->_retainCount = 1;
	deck->_cards = CreateCardList();
	deck->_discarded = CreateCardList();
	deck->_nextDeck = nullptr;
	deck->_previousDeck = nullptr;
	
	if (!emptyDeck)
	{
		int uniqueId = 0;
		for (int decks = 0; decks < kDecksToShuffle; decks++)
		{
			// we only want 2 jokers, so don't include them here
			for (CardSuit suit = CardSuit::Min; suit <= CardSuit::Max; IterateCardSuit(suit))
			{
				if (suit == CardSuit::Joker) continue;
				for (CardNumber number = CardNumber::Min; number <= CardNumber::Max; IterateCardNumber(number))
				{
					if (number == CardNumber::Joker) continue;
					TGMCard* card = CreateCard(uniqueId++, number, suit);
					CardListTransfer(deck->_cards, card);
				}
			}
			// just make jokers the 
			for (int jokers = 0; jokers < kNumberOfJokersPerDeck; jokers++)
			{
				TGMCard* card = CreateCard(uniqueId++, CardNumber::Joker, CardSuit::Joker);
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
	while (card != nullptr)
	{
		TGMCard* copy = CopyCard(card);
		CardListTransfer(deckCopy->_cards, copy);
		card = card->nextCard;
	}
	
	card = FirstCardNoRef(deck->_discarded);
	while (card != nullptr)
	{
		TGMCard* copy = CopyCard(card);
		CardListTransfer(deckCopy->_discarded, copy);
		card = card->nextCard;
	}
	
	return deckCopy;
}

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
		memset(deck, static_cast<int>(0xDEADBEEF), sizeof(TGMDeck));
		deck->_retainCount = 0;
#endif
		MemDecreaseGlobalCount(g_decksLiving);
		free(deck);
	}
}

void DeckShuffle(TGMDeck* deck, TGRandom* rng)
{
#if 1
	TGMCard *card = FirstCardNoRef(deck->_discarded);
	while (card != nullptr) [[likely]]
	{
		TGMCard *next = card->nextCard;
		TransferCardFromList(card, deck->_discarded, deck->_cards);
		card = next;
	}
	
	unsigned count = CardListCount(deck->_cards);
	dassert(count == DeckCapacity());

	// for shuffling, we first create an array of items
	card = FirstCardNoRef(deck->_cards);
	unsigned i = 0;
	while (card != nullptr) [[likely]]
	{
		deck->_fastShuffle[i++] = card;
		card = card->nextCard;
	}
	
	TGMCard *last = nullptr;
	TGMCard *head = nullptr;
	TGMCard *c = nullptr;
	while (count > 0)
	{
		unsigned cardPos = RandomRandom(rng) % count; // Shuffling
		c = deck->_fastShuffle[cardPos];
		deck->_fastShuffle[cardPos] = deck->_fastShuffle[(count - 1)];
		c->nextCard = head;
		if (head != nullptr) [[likely]]
		{
			head->previousCard = c;
		}
		c->previousCard = nullptr;
		head = c;
		last = last != nullptr ? last : c;
		--count;
	}
	
	deck->_cards->_first = head;
	deck->_cards->_last = last;
#else
	TGMCard *card = FirstCardNoRef(deck->_discarded);
	while (card != nullptr)
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
#endif
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
	while (card != nullptr)
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
	if (deck1 != nullptr && deck2 == nullptr) return false;
	if (deck1 == nullptr && deck2 != nullptr) return false;
	// stupid branch for analyzer
	if (deck1 == nullptr || deck2 == nullptr) return true;
	
	if (CardListCount(deck1->_cards) != CardListCount(deck2->_cards)) return false;
	if (CardListCount(deck1->_discarded) != CardListCount(deck2->_discarded)) return false;
	
	TGMCard* thisCard;
	TGMCard* thatCard;
	
	thisCard = FirstCardNoRef(deck1->_cards);
	thatCard = FirstCardNoRef(deck2->_cards);
	while (thisCard != nullptr)
	{
		if (!AreCardsEqual(thisCard, thatCard)) return false;
		thisCard = thisCard->nextCard;
		thatCard = thatCard->nextCard;
	}
	
	thisCard = deck1->_discarded->_first;
	thatCard = deck2->_discarded->_first;
	while (thisCard != nullptr)
	{
		if (!AreCardsEqual(thisCard, thatCard)) return false;
		thisCard = thisCard->nextCard;
		thatCard = thatCard->nextCard;
	}
	
	return true;
}

