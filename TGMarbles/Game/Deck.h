//
//  Deck.h
//  Marbles
//
//  Created by todd harris on 11/27/11.
//  Copyright 2012 toadgee.com. All rights reserved.
//

#pragma once
#include "Card.h"
#include "CardList.h"
#include "Random.h"

#define kDecksToShuffle			2 // number of decks

// Represents a Marbles deck (or two regular decks of cards including jokers)
#define TGMDeck struct TGMDeckStruct
// NOTE : versioned
struct TGMDeckStruct
{
	// order and typing important because of "base" struct
	int32_t _retainCount;
	TGMDeck* _nextDeck;
	TGMDeck* _previousDeck;
	
	TGMCardList* _cards;
	TGMCardList* _discarded;
};

TGMDeck* CreateNonEmptyDeck(void);
TGMDeck* CreateDeck(bool emptyDeck);
TGMDeck* CopyDeck(TGMDeck *deck);
#if 0
void RetainDeck(TGMDeck* deck);
#else
#define RetainDeck(deck) MemIncreaseRetainCount(deck->_retainCount)
#endif
void ReleaseDeck(TGMDeck *deck);

void DeckShuffle(TGMDeck* deck, TGRandom* rng);
TGMCard* DeckRemoveTopCard(TGMDeck* deck); // card is now owned by caller
bool DeckIsEmpty(TGMDeck* deck);
void DeckClear(TGMDeck* deck);

void DeckAddDiscard(TGMDeck* deck, TGMCard* discard);

bool AreDecksEqual(TGMDeck* deck1, TGMDeck* deck2);

uint16_t DeckCapacity();
