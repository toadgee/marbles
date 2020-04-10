//
//	TGMBoardView.h
//	Marbles
//
//	Created by Todd on 10/17/12.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Board.h"
#import "Game.h"
#import "TouchSpot.h"

#define kPointsArraySize 168  // 108 + final 30 spots + home 30 spots - doesn't include negative marker spot or middle spot

typedef enum
{
	BoardViewEventType_Unknown = 1200,
	BoardViewEventType_Spot1Cleared = 1201,
	BoardViewEventType_Spot2Cleared = 1202,
	BoardViewEventType_Spot1Changed = 1203,
	BoardViewEventType_Spot2Changed = 1204,
	BoardViewEventType_Spot1Added =	  1205,
	BoardViewEventType_Spot2Added =	  1206,
} BoardViewEventType;


@class TGMBoardView;
@protocol TGMBoardViewDelegate < NSObject >
-(void)boardView:(TGMBoardView *)boardView touchedSpot:(TGMTouchSpot *)spot eventType:(BoardViewEventType)event;
@end


@interface TGMBoardView : UIView
{
	NSArray *_validTouchSpots; // spots that the user can touch to activate
	
	CGFloat _circleSize;
	UIColor *_holeColor;
	UIColor *_highlightColor1;
	UIColor *_highlightColor2;
	UIColor *_highlightColor3;
	int _highlightedSpot1;
	int _highlightedSpot2;
	int _spotCtr;
	CGRect _spotRects[kPointsArraySize];
	
	UIColor *_redColor;
	UIColor *_yellowColor;
	UIColor *_greenColor;
	UIColor *_blueColor;
	UIColor *_whiteColor;
	UIColor *_blackColor;
	
	CGPoint _a;
	CGPoint _b;
	CGPoint _c;
	CGPoint _d;
	CGPoint _e;
	CGPoint _f;
	CGPoint _g;
	CGPoint _h;
	CGPoint _i;
	CGPoint _j;
	CGPoint _k;
	CGPoint _l;
	CGPoint _m;
	CGPoint _n;
	CGPoint _o;
	CGPoint _p;
	CGPoint _q;
	CGPoint _r;
	CGPoint _s;
	CGPoint _t;
	CGPoint _u;
	CGPoint _v;
	CGPoint _w;
	CGPoint _x;
	
	CGPoint _fp1;
	CGPoint _fp2;
	CGPoint _fp3;
	CGPoint _fp4;
	CGPoint _fp5;
	CGPoint _fp6;
	
	CGPoint _playerMarker1;
	CGPoint _playerMarker2;
	CGPoint _playerMarker3;
	CGPoint _playerMarker4;
	CGPoint _playerMarker5;
	CGPoint _playerMarker6;
}

@property (readwrite, assign, nonatomic) TGMGame* game;
@property (readwrite, weak, nonatomic) IBOutlet id<TGMBoardViewDelegate> delegate;
@property (readonly, strong, nonatomic) TGMTouchSpot* spot1;
@property (readonly, strong, nonatomic) TGMTouchSpot* spot2;
@property (readwrite, strong, nonatomic) NSArray<TGMTouchSpot *> *validTouchSpots;

- (CGPoint)pointForPlayerColor:(PlayerColor)pc;
- (void)highlightSpot1:(TGMTouchSpot *)spot1;
- (void)clearHighlights;
- (UIColor *)colorForPlayerColor:(PlayerColor)pc;

@end
