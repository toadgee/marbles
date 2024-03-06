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
	TGMDeck* _nextDeck;
	TGMDeck* _previousDeck;
	
	TGMCardList* _cards;
	TGMCardList* _discarded;
	
	TGMCard* _fastShuffle[108];
	
	int32_t _retainCount;
	char _padding[4];
};

TGMDeck* CreateNonEmptyDeck(void);
TGMDeck* CreateDeck(bool emptyDeck);
TGMDeck* CopyDeck(TGMDeck *deck);
#define RetainDeck(deck) MemIncreaseRetainCount(deck->_retainCount)
void ReleaseDeck(TGMDeck *deck);

void DeckShuffle(TGMDeck* deck, TGRandom* rng);
TGMCard* DeckRemoveTopCard(TGMDeck* deck); // card is now owned by caller
bool DeckIsEmpty(TGMDeck* deck);
void DeckClear(TGMDeck* deck);

void DeckAddDiscard(TGMDeck* deck, TGMCard* discard);

bool AreDecksEqual(TGMDeck* deck1, TGMDeck* deck2);

uint16_t DeckCapacity();
