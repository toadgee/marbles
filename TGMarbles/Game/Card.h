//
//  Card.h
//  Marbles
//
//  Created by todd harris on 11/27/11.
//  Copyright 2012 toadgee.com. All rights reserved.
//

#pragma once

#include "CardNumber.h"
#include "CardSuit.h"

enum class TGMCardComparisonResult: uint16_t
{
	Same,
	Ascending,
	Descending
};

#define TGMCard struct TGMCardStruct
// NOTE : versioned

struct TGMCardStruct
{
	TGMCard* nextCard;
	
	TGMCard* previousCard;
	
	void* _holder;
	
	int32_t _retainCount;
	int _uniqueId;
	
	CardSuit _suit;
	CardNumber _number;
	char padding[4];
};

#define CardUniqueId(card) (card->_uniqueId)
#define CardNumber(card) (card->_number)
#define CardSuit(card) (card->_suit)

TGMCard* CreateCard(int uniqueId, CardNumber number, CardSuit suit);
#define CopyCard(card) CreateCard(card->_uniqueId, card->_number, card->_suit)
#define RetainCard(card) MemIncreaseRetainCount(card->_retainCount)
#define ReleaseCard(card) if ((MemDecreaseRetainCount(card->_retainCount)) == 0) DeallocateCard(card)
void DeallocateCard(TGMCard* card);

bool AreCardsEqual(TGMCard* card1, TGMCard* card2);
bool AreCardsEquivalent(TGMCard* card1, TGMCard* card2);

// non strict will ignore the suit & unique Id
TGMCardComparisonResult CompareCards(TGMCard* card1, TGMCard* card2);
TGMCardComparisonResult CompareCardsForDiscard(TGMCard* card1, TGMCard* card2);


