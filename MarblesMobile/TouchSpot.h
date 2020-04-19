//
//  TouchSpot.h
//  Marbles
//
//  Created by toddha on 12/19/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Move.h"

@interface TGMTouchSpot : NSObject
@property (readonly, assign, nonatomic) int spot;
@property (readonly, assign, nonatomic) PlayerColor playerColor;
@property (readonly, assign, nonatomic, getter=isFinalSpot) BOOL finalSpot;
@property (readonly, assign, nonatomic, getter=isHomeSpot) BOOL homeSpot;

+(TGMTouchSpot*)touchSpotFromMoveBegin:(TGMMove *)move;
+(TGMTouchSpot*)touchSpotFromMoveEnd:(TGMMove *)move;

+(instancetype)touchSpotWithSpot:(int)spot playerColor:(PlayerColor)pc isFinalSpot:(BOOL)isFinal isHomeSpot:(BOOL)isHome;
-(instancetype)initWithSpot:(int)spot playerColor:(PlayerColor)pc isFinalSpot:(BOOL)isFinal isHomeSpot:(BOOL)isHome;

-(BOOL)isEqualToSpot:(TGMTouchSpot*)other;
@end
