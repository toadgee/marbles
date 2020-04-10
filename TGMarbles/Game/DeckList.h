//
//  DeckList.h
//  Marbles
//
//  Created by Todd Harris on 2/18/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#pragma once
#include "Deck.h"

typedef std::function<bool(unsigned i, TGMDeck* deck)> TGMDeckListIterationBlock;

#define TGMDeckList struct TGMDeckListStruct

struct TGMDeckListStruct
{
	int32_t _retainCount;
	
	TGMDeck* _first;
	TGMDeck* _last;
	unsigned _count;
};

TGMDeckList* CreateDeckList(void);
TGMDeckList* RetainDeckList(TGMDeckList* deckList);
void ReleaseDeckList(TGMDeckList* deckList);

void DeckListAdd(TGMDeckList* deckList, TGMDeck* deck);
TGMDeck* DeckListRemoveFirst(TGMDeckList* deckList); // owns reference
TGMDeck* DeckListDeckAtIndex(TGMDeckList* deckList, uint16_t index, bool ownRef);

void DeckListIterateWithBlock(TGMDeckList* deckList, TGMDeckListIterationBlock block);

#define DeckListCount(deckList) deckList->_count
