//
//  DataExtensions.m
//  Marbles
//
//  Created by Todd on 1/8/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#include "DebugAssert.h"
#include "MarblesMem.h"

#define kCardDataVersion 1
#define kDeckDataVersion 1
#define kGameLogDataVersion 1
#define kMarbleDataVersion 1
#define kMoveDataVersion 1

#define kCardDataMarker 1000
#define kDeckDataMarker 1001
#define kGameLogDataMarker 1002
#define kMarbleDataMarker 1003
#define kMoveDataMarker 1004

#define kMarblesMajorVersion 1
#define kMarblesMinorVersion 0
#define kMarblesBuildVersion 0

#import "DataExtensions.h"

@interface NSData (MarblesExtensions)
-(BOOL)boolAtOffset:(uint16_t *)offset;
-(int)intAtOffset:(uint16_t *)offset;
-(int16_t)int16AtOffset:(uint16_t *)offset;
-(CardNumber)cardNumberAtOffset:(uint16_t *)offset;
-(CardSuit)cardSuitAtOffset:(uint16_t *)offset;
-(MarbleColor)marbleColorAtOffset:(uint16_t *)offset;
-(PlayerColor)playerColorAtOffset:(uint16_t *)offset;
-(MarbleStrategy)strategyAtOffset:(uint16_t *)offset;
@end

@interface NSMutableData (MarblesExtensions)
-(void)appendBool:(BOOL)value;
-(void)appendInt:(int)value;
-(void)appendInt16:(int16_t)value;
-(void)appendCardNumber:(CardNumber)value;
-(void)appendCardSuit:(CardSuit)value;
-(void)appendMarbleColor:(MarbleColor)value;
-(void)appendPlayerColor:(PlayerColor)value;
-(void)appendMarbleStrategy:(MarbleStrategy)value;
@end

@implementation NSData (MarblesExtensions)

#define typeAtOffset(type) \
    NSRange rng = NSMakeRange(*offset, sizeof(type)); \
    *offset += sizeof(type); \
    type value; \
    [self getBytes:(void*)&value range:rng]; \
    return value

-(BOOL)boolAtOffset:(uint16_t *)offset { typeAtOffset(BOOL); }
-(int16_t)int16AtOffset:(uint16_t *)offset { typeAtOffset(int16_t); }
-(int)intAtOffset:(uint16_t *)offset { typeAtOffset(int); }
-(CardNumber)cardNumberAtOffset:(uint16_t *)offset { typeAtOffset(CardNumber); }
-(CardSuit)cardSuitAtOffset:(uint16_t *)offset { typeAtOffset(CardSuit); }
-(MarbleColor)marbleColorAtOffset:(uint16_t *)offset { typeAtOffset(MarbleColor); }
-(PlayerColor)playerColorAtOffset:(uint16_t *)offset { typeAtOffset(PlayerColor); }
-(MarbleStrategy)strategyAtOffset:(uint16_t *)offset { typeAtOffset(MarbleStrategy); }
@end

#define appendType(type, value) [self appendBytes:(void*)&value length:sizeof(type)]

@implementation NSMutableData (MarblesExtensions)
-(void)appendBool:(BOOL)value { appendType(BOOL, value); }
-(void)appendInt:(int)value { appendType(int, value); }
-(void)appendInt16:(int16_t)value { appendType(int16_t, value); }
-(void)appendCardNumber:(CardNumber)value { appendType(CardNumber, value); }
-(void)appendCardSuit:(CardSuit)value { appendType(CardSuit, value); }
-(void)appendMarbleColor:(MarbleColor)value { appendType(MarbleColor, value); }
-(void)appendPlayerColor:(PlayerColor)value { appendType(PlayerColor, value); }
-(void)appendMarbleStrategy:(MarbleStrategy)value { appendType(MarbleStrategy, value); }
@end

uint16_t MarbleDataLength()
{
	return (
		sizeof(int) // marker
		 + sizeof(int) // version
		 + sizeof(bool) // null marble
		 + sizeof(MarbleColor) // color
		 + sizeof(int16_t) // distanceFromHome
		 + sizeof(bool)); // wentBehindHome
}

uint16_t CardDataLength()
{
	return (
		sizeof(int)  // marker
		+ sizeof(int) // version
		+ sizeof(int) // unique Id
		+ sizeof(CardNumber) // number
		+ sizeof(CardSuit)); // suit
}

TGMCard* CreateCardFromData(TGMData *data, uint16_t* offset)
{
	if (offset == NULL || [data length] < CardDataLength() + *offset)
	{
		dassert(false);
		return NULL;
	}
	
	int marker = [data intAtOffset:offset];
	if (marker != kCardDataMarker)
	{
		dassert(false);
		return NULL;
	}
	
	int version = [data intAtOffset:offset];
	if (version != kCardDataVersion)
	{
		dassert(false);
		return NULL;
	}
	
	int uniqueId = [data intAtOffset:offset];
	CardNumber cn = [data cardNumberAtOffset:offset];
	CardSuit cs = [data cardSuitAtOffset:offset];
	return CreateCard(uniqueId, cn, cs);
}

TGMData* GetCardData(TGMCard *card)
{
	NSMutableData* data = [NSMutableData dataWithCapacity:CardDataLength()];
	[data appendInt:kCardDataMarker];
	[data appendInt:kCardDataVersion];
	[data appendInt:card->_uniqueId];
	[data appendCardNumber:card->_number];
	[data appendCardSuit:card->_suit];
	return data;
}

TGMData* DeckData(TGMDeck *deck)
{
	NSMutableData* data = [NSMutableData data];
	[data appendInt:kDeckDataMarker];
	[data appendInt:kDeckDataVersion];
	[data appendInt:(int)CardListCount(deck->_cards)];
	CardListIterateWithBlock(deck->_cards, ^(unsigned i, TGMCard *card)
	{
		[data appendInt:(int)i];
		[data appendData:GetCardData(card)];
		return true;
	});
	
	[data appendInt:(int)CardListCount(deck->_discarded)];
	CardListIterateWithBlock(deck->_discarded, ^(unsigned i, TGMCard *card)
	{
		[data appendInt:(int)i];
		[data appendData:GetCardData(card)];
		return true;
	});
	
	return data;
}

TGMDeck* CreateDeckFromData(TGMData *data, uint16_t* offset)
{
	if (offset == NULL)
	{
		dassert(false);
		return NULL;
	}
	
	int marker = [data intAtOffset:offset];
	if (marker != kDeckDataMarker)
	{
		dassert(false);
		return NULL;
	}
	
	int version = [data intAtOffset:offset];
	if (version != kDeckDataVersion)
	{
		dassert(false);
		return NULL;
	}
	
	TGMDeck* deck = CreateDeck(true);
	
	int cardCount = [data intAtOffset:offset];
	for (int i = 0; i < cardCount; i++)
	{
		int j = [data intAtOffset:offset];
		if (i != j)
		{
			dassert(false);
			ReleaseDeck(deck);
			return NULL;
		}
		
		TGMCard* card = CreateCardFromData(data, offset);
		if (card == NULL)
		{
			dassert(false);
			ReleaseDeck(deck);
			return NULL;
		}
		
		CardListTransfer(deck->_cards, card);
	}
	
	int discardCount = [data intAtOffset:offset];
	for (int i = 0; i < discardCount; i++)
	{
		int j = [data intAtOffset:offset];
		if (i != j)
		{
			dassert(false);
			ReleaseDeck(deck);
			return NULL;
		}
		
		TGMCard* card = CreateCardFromData(data, offset);
		if (card == NULL)
		{
			dassert(false);
			ReleaseDeck(deck);
			return NULL;
		}
		
		CardListTransfer(deck->_discarded, card);
	}
	
	return deck;
}

TGMGameLog* CreateGameLogFromData(TGMData *data)
{
	uint16_t offset = 0;
	
	// gets the major, minor, build versions
	[data intAtOffset:&offset];
	[data intAtOffset:&offset];
	[data intAtOffset:&offset];
	
	int marker = [data intAtOffset:&offset];
	if (marker != kGameLogDataMarker)
	{
		dassert(false);
		return nullptr;
	}
	
	int version = [data intAtOffset:&offset];
	if (version != kGameLogDataVersion)
	{
		dassert(false);
		return nullptr;
	}
	
	TGMGameLog* gameLog = CreateGameLog();
	PlayerColor pc = (PlayerColor)[data intAtOffset:&offset];
	GameLogSetDealingPlayer(gameLog, pc);
	
	for (unsigned p = 0; p < kPlayers; p++)
	{
		MarbleStrategy strategy = [data strategyAtOffset:&offset];
		if (!IsValidStrategy(strategy))
		{
			dassert(false);
			return nil;
		}
		
		gameLog->_playerStrategy[p] = strategy;
	}
	
	uint16_t deckCount = (uint16_t)[data intAtOffset:&offset];
	for (uint16_t d = 0; d < deckCount; d++)
	{
		uint16_t dEncoded = (uint16_t)[data intAtOffset:&offset];
		if (d != dEncoded)
		{
			dassert(false);
			return nil;
		}
		
		TGMDeck* deck = CreateDeckFromData(data, &offset);
		if (!deck)
		{
			dassert(false);
			return nil;
		}
		
		DeckListAdd(gameLog->_deckList, deck);
		ReleaseDeck(deck);
	}
	
	int moveCount = [data intAtOffset:&offset];
	for (int i = 0; i < moveCount; i++)
	{
		int iEnc = [data intAtOffset:&offset];
		if (i != iEnc)
		{
			dassert(false);
			return nil;
		}
		
		TGMMove* move = CreateMoveFromData(data, &offset);
		if (move == NULL)
		{
			dassert(false);
			return nil;
		}
		
		dassert(gameLog != nil);
		if (gameLog != nil)
		{
			MoveListTransfer(gameLog->_moveList, move);
		}
		else
		{
			ReleaseMove(move);
		}
	}
	
	return gameLog;
}

TGMData* GameLogData(TGMGameLog* gameLog)
{
	// not perfect as it's not the serialized move size, but close enough
	NSMutableData *data = [NSMutableData dataWithCapacity:(sizeof(int) + MoveListCount(gameLog->_moveList) * sizeof(TGMMove))];
	
	// version
	[data appendInt:kMarblesMajorVersion];
	[data appendInt:kMarblesMinorVersion];
	[data appendInt:kMarblesBuildVersion];
	
	[data appendInt:kGameLogDataMarker];
	[data appendInt:kGameLogDataVersion];
	
	[data appendPlayerColor:gameLog->_dealingPlayer];
	
	for (unsigned p = 0; p < kPlayers; p++)
	{
		[data appendMarbleStrategy:gameLog->_playerStrategy[p]];
	}
	
	[data appendInt:(int)DeckListCount(gameLog->_deckList)];
	DeckListIterateWithBlock(gameLog->_deckList, ^(unsigned d, TGMDeck* deck)
	{
		[data appendInt:(int)d];
		[data appendData:DeckData(deck)];
		return YES;
	});
	
	[data appendInt:(int)MoveListCount(gameLog->_moveList)];
	MoveListIterateWithBlock(gameLog->_moveList,^(int i, TGMMove *move)
	{
		[data appendInt:i];
		[data appendData:GetMoveData(move)];
	});
	
	return data;
}

TGMMarble* CreateMarbleFromData(TGMData *data, uint16_t* offset, bool* nullMarbleEncoded)
{
	if (offset == NULL || [data length] < MarbleDataLength() + *offset)
	{
		dassert(false);
		return NULL;
	}
	
	int marker = [data intAtOffset:offset];
	if (marker != kMarbleDataMarker)
	{
		dassert(false);
		return NULL;
	}
	
	int version = [data intAtOffset:offset];
	if (version != kMarbleDataVersion)
	{
		dassert(false);
		return NULL;
	}
	
	BOOL nullMarble = [data boolAtOffset:offset];
	MarbleColor mc = [data marbleColorAtOffset:offset];
	int16_t distanceFromHome = [data int16AtOffset:offset];
	BOOL wentBehindHome = [data boolAtOffset:offset];
	
	if (nullMarbleEncoded)
	{
		*nullMarbleEncoded = nullMarble;
	}
	
	if (nullMarble)
	{
		return NULL;
	}
	
	return CreateMarble(mc, distanceFromHome, wentBehindHome);
}

TGMData* GetMarbleData(TGMMarble *marble)
{
	NSMutableData* data = [NSMutableData dataWithCapacity:MarbleDataLength()];
	[data appendInt:kMarbleDataMarker];
	[data appendInt:kMarbleDataVersion];
	
	BOOL isNullMarble = YES;
	MarbleColor mc = Color_None;
	int16_t distanceFromHome = 0;
	BOOL wentBehindHome = NO;
	
	if (marble)
	{
		isNullMarble = NO;
		mc = marble->color;
		distanceFromHome = marble->distanceFromHome;
		wentBehindHome = marble->wentBehindHome;
	}
	
	
	[data appendBool:isNullMarble];
	[data appendMarbleColor:mc];
	[data appendInt16:distanceFromHome];
	[data appendBool:wentBehindHome];
	
	return data;
}

TGMMove* CreateMoveFromData(TGMData *data, uint16_t* offset)
{
	if (offset == NULL || [data length] < MoveDataLength() + *offset)
	{
		dassert(false);
		return NULL;
	}
	
	int marker = [data intAtOffset:offset];
	if (marker != kMoveDataMarker)
	{
		dassert(false);
		return NULL;
	}
	
	int version = [data intAtOffset:offset];
	if (version != kMoveDataVersion)
	{
		dassert(false);
		return NULL;
	}

	bool nullMarbleEncoded = false;
	TGMMarble* marble = CreateMarbleFromData(data, offset, &nullMarbleEncoded);
	if (marble == NULL && !nullMarbleEncoded)
	{
		dassert(false);
		return NULL;
	}
	
	TGMCard* card = CreateCardFromData(data, offset);
	if (card == NULL)
	{
		dassert(false);
		return NULL;
	}
	
	PlayerColor playerColor = [data playerColorAtOffset:offset];
	int oldSpot = [data intAtOffset:offset];
	int newSpot = [data intAtOffset:offset];
	BOOL isDiscard = [data boolAtOffset:offset];
	int moves = [data intAtOffset:offset];
	BOOL wentBehindHome = [data boolAtOffset:offset];
	int jumps = [data intAtOffset:offset];
	TGMMove* move = MakeMove(card, marble, isDiscard, playerColor, oldSpot, newSpot, moves, jumps, wentBehindHome);
	if (marble != NULL) ReleaseMarble(marble);
	ReleaseCard(card);
	return move;
}

inline uint16_t MoveDataLength()
{
	return sizeof(int) // marker
		+ sizeof(int) // version
		+ MarbleDataLength() // marble
		+ CardDataLength() // card
		+ sizeof(PlayerColor) // color
		+ sizeof(int) // old spot
		+ sizeof(int) // new spot
		+ sizeof(bool) // isDiscard
		+ sizeof(int) // moves
		+ sizeof(bool) // went behind home
		+ sizeof(int) ; // jumps
}

TGMData* GetMoveData(TGMMove *move)
{
	NSMutableData* data = [NSMutableData dataWithCapacity:MoveDataLength()];
	[data appendInt:kMoveDataMarker];
	[data appendInt:kMoveDataVersion];
	[data appendData:GetMarbleData(move->marble)];
	[data appendData:GetCardData(move->card)];
	[data appendPlayerColor:move->playerColor];
	[data appendInt:move->oldSpot];
	[data appendInt:move->newSpot];
	[data appendBool:move->isDiscard];
	[data appendInt:move->moves];
	[data appendBool:move->wentBehindHome];
	[data appendInt:move->jumps];
	return data;
}

