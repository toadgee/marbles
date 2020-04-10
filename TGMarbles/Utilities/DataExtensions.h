//
//  DataExtensions.h
//  Marbles
//
//  Created by Todd on 1/8/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#include "GameLog.h"
#include "Deck.h"
#include "Card.h"

uint16_t MoveDataLength();
uint16_t CardDataLength();
uint16_t MarbleDataLength();

#if __OBJC__
#if TARGET_OS_OSX || TARGET_OS_IPHONE // TGMData
#define TGMData NSData // temporary
TGMGameLog* CreateGameLogFromData(TGMData *data);
TGMData* GameLogData(TGMGameLog* gameLog);
TGMCard* CreateCardFromData(TGMData *data, uint16_t* offset);
TGMData* GetCardData(TGMCard *card);
TGMData* DeckData(TGMDeck *deck);
TGMDeck* CreateDeckFromData(TGMData *data, uint16_t* offset);
TGMData* GetMarbleData(TGMMarble *marble);
TGMMove* CreateMoveFromData(TGMData *data, uint16_t* offset);
TGMData* GetMoveData(TGMMove *move);
#endif
#endif
