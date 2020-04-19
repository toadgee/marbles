//
//  TouchSpot.m
//  Marbles
//
//  Created by toddha on 12/19/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import "precomp.h"
#import "DebugAssert.h"
#import "Descriptions.h"
#import "TouchSpot.h"

@implementation TGMTouchSpot

+(TGMTouchSpot*)touchSpotFromMoveBegin:(TGMMove *)move
{
	if (move->isDiscard) return nil;
	if (move->oldSpot == kGetOutSpot)
	{
		// TODO [choose specific home pieces] - choose an actual marble
		return [TGMTouchSpot touchSpotWithSpot:0 playerColor:move->playerColor isFinalSpot:NO isHomeSpot:YES];
	}
	else if (IsFinalSpot(move->oldSpot))
	{
		return [TGMTouchSpot touchSpotWithSpot:SpotToFinalSpot(move->oldSpot) playerColor:move->playerColor isFinalSpot:YES isHomeSpot:NO];
	}
	
	return [TGMTouchSpot touchSpotWithSpot:move->oldSpot playerColor:PlayerColor::None isFinalSpot:NO isHomeSpot:NO];
}

+(TGMTouchSpot*)touchSpotFromMoveEnd:(TGMMove *)move
{
	if (move->isDiscard) return nil;
	if (IsFinalSpot(move->newSpot))
	{
		return [TGMTouchSpot touchSpotWithSpot:SpotToFinalSpot(move->newSpot) playerColor:move->playerColor isFinalSpot:YES isHomeSpot:NO];
	}
	
	return [TGMTouchSpot touchSpotWithSpot:move->newSpot playerColor:PlayerColor::None isFinalSpot:NO isHomeSpot:NO];
}

+(instancetype)touchSpotWithSpot:(int)spot playerColor:(PlayerColor)pc isFinalSpot:(BOOL)isFinal isHomeSpot:(BOOL)isHomeSpot
{
	return [[TGMTouchSpot alloc] initWithSpot:spot playerColor:pc isFinalSpot:isFinal isHomeSpot:isHomeSpot];
}

-(instancetype)initWithSpot:(int)spot playerColor:(PlayerColor)pc isFinalSpot:(BOOL)isFinalSpot isHomeSpot:(BOOL)isHomeSpot
{
	self = [super init];
	if (self)
	{
		if (isFinalSpot)
		{
            dassert(IsValidFinalSpot(spot));
		}
		else if (isHomeSpot)
		{
            dassert(IsValidHomeSpot(spot));
		}
		else
		{
            dassert(IsValidSpot(spot));
		}
		
		_spot = spot;
		_playerColor = pc;
		_finalSpot = isFinalSpot;
		_homeSpot = isHomeSpot;
	}
	return self;
}

- (NSString *)description
{
	if (_finalSpot)
	{
		return [NSString stringWithFormat:@"Final Spot %d for %s", _spot, PlayerColorToString(_playerColor)];
	}
	else if (_homeSpot)
	{
		return [NSString stringWithFormat:@"Home Spot %d for %s", _spot, PlayerColorToString(_playerColor)];
	}
	
	return [NSString stringWithFormat:@"Spot %d", _spot];
}

-(BOOL)isEqualToSpot:(TGMTouchSpot*)other
{
	return ((self.spot == other.spot || self.isHomeSpot) // [choose specific home pieces] need to just ignore if it's a home spot for now
		&& self.playerColor == other.playerColor
		&& self.isFinalSpot == other.isFinalSpot
		&& self.isHomeSpot == other.isHomeSpot);
}

@end
