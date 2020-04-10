//
//	BoardView.m
//	Marbles
//
//	Created by Todd on 10/17/12.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import "MarblesCommon.h"
#import "BoardView.h"
#import <math.h>
#import "UIImageExtensions.h"
#import "Player.h"

#define DRAW_LINES 0   // this draws lines between all points
#define DRAW_LETTERS 0 // this draws the points we use to calculate subpoints
#define DRAW_NUMBERS 0 // this draws the indices of the points in _spotRects (which is different than board numbers)
#define DRAW_BOARD_NUMBERS 0 // this makes DRAW_NUMBERS draw the real board numbers (for only regular spots)
#define DRAW_PLAYER_MARKERS 0 // this draws the spots where the turn marker for each player goes
#define DRAW_PLAYER_POINTS 0 // this draws the "point" at which a player is recognized

#define kPointsOffsetToBoard -3 // the board starts 3 off where we start
#define kRegularBoardPointsStarting 0 // the first 108
#define kFinalPointsStarting 108
#define kHomePointsStarting 138

#define kCircleAdjust 0.9f

#define kHighlightExtraPixels 5

@implementation TGMBoardView
- (instancetype)init
{
	self = [super init];
	if (self)
	{
		[self internalSetup];
	}
	return self;
}

- (instancetype)initWithCoder:(NSCoder *)decoder
{
	self = [super initWithCoder:decoder];
	if (self)
	{
		[self internalSetup];
	}
	return self;
}

- (instancetype)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];
	if (self)
	{
		[self internalSetup];
	}
	return self;
}

- (void)internalSetup
{
	_highlightedSpot1 = -1;
	_highlightedSpot2 = -1;
	_highlightColor1 = [[UIColor alloc] initWithRed:0.8f green:1.0f blue:0.8f alpha:0.35f];
	_highlightColor2 = [[UIColor alloc] initWithRed:1.0f green:0.8f blue:0.8f alpha:0.35f];
	_highlightColor3 = [[UIColor alloc] initWithRed:1.0f green:1.0f blue:1.0f alpha:0.4f];
	_holeColor =       [[UIColor alloc] initWithRed:0.2f green:0.14f blue:0.0f alpha:0.35f];
	_redColor =        [[UIColor alloc] initWithRed:0.9f green:0.0f blue:0.0f alpha:1.0f];
	_yellowColor =     [UIColor yellowColor];
	_greenColor =      [[UIColor alloc] initWithRed:0.0f green:0.7f blue:0.0f alpha:1.0f];
	_blueColor =       [[UIColor alloc] initWithRed:0.0f green:0.0f blue:0.9f alpha:1.0f];
	_whiteColor =      [[UIColor alloc] initWithRed:1.0f green:1.0f blue:1.0f alpha:1.0f];
	_blackColor =      [[UIColor alloc] initWithRed:0.0f green:0.0f blue:0.0f alpha:1.0f];
	
	self.backgroundColor = [UIColor clearColor];
	
	[self setupPoints];
}

- (CGPoint)pointForPlayerColor:(PlayerColor)playerColor
{
	CGPoint point = CGPointZero;
	for (int i = kHomePointsStarting; i < kPointsArraySize; i++)
	{
		int base = (i - kHomePointsStarting);
		int marble = base % kMarblesPerPlayer;
		if (marble != (kMarblesPerPlayer / 2))
		{
			continue;
		}
		
		int player = base / kMarblesPerPlayer;
		PlayerColor pc = PlayerColorForPosition(player);
		if (pc != playerColor)
		{
			continue;
		}
		
		CGRect spotRect = _spotRects[i];
		dassert(!CGRectIsNull(spotRect));
		point = spotRect.origin;
	}
	
	return point;
}

#define kPi 3.141529
#define kThirtyDegreesInRadians (kPi / 6.0)
#define kSixtyDegreesInRadians (kPi / 3.0)
#define kSin30 (sin(kThirtyDegreesInRadians))
#define kCos30 (cos(kThirtyDegreesInRadians))
#define kTan30 (tan(kThirtyDegreesInRadians))
#define kSin60 (sin(kSixtyDegreesInRadians))
#define kCos60 (cos(kSixtyDegreesInRadians))
-(void)setupPoints
{
	// generate A to X
	{
		// determine smallest side of view to determine how big the height is
		CGRect bounds = self.frame;
		CGFloat smallestSide = bounds.size.height;
		if (bounds.size.width < smallestSide)
		{
			smallestSide = bounds.size.width;
		}
	
		// determine height of the hexagon [ h = 2s / (1 + 2tan30)]
		CGFloat height = (CGFloat)((2 * smallestSide) / (1 + (2 * kTan30)));
		height = 0.97f * height;
	
		// we want it to be centered in the view, so find the center point of the screen
		CGFloat xAdj = (bounds.size.width / 2);
		CGFloat yAdj = (bounds.size.height / 2);
	
		CGFloat h = height / 2;
		// TODO : adjusting m screws up the board - which is where the issue lies
		CGFloat m = h / 2; // the spoke width in relation to the height
	
		// these are all the base coordinates for points k - p + w, on which all other points are based
		CGFloat kx = 0;
		CGFloat ky = h;
		CGFloat lx = m / 2;
		CGFloat ly = h;
		CGFloat mx = m / 2;
		CGFloat my = h - m;
		CGFloat nx = (CGFloat)((m / 2) + (m * kSin60));
		CGFloat ny = (CGFloat)(h - m * (1 - kCos60));
		CGFloat ox = (CGFloat)((h * kSin60) + ((m / 2) * kSin30));
		CGFloat oy = (CGFloat)((h * kSin30) - ((m / 2) * kCos30));
		CGFloat px = (CGFloat)((h * kSin60) + ((m / 2) * kSin30) - (m * kCos30));
		CGFloat py = 0;
		CGFloat wx = (CGFloat)(h * kSin60);
		CGFloat wy = (CGFloat)(h * kSin30);
		CGFloat fp5x = mx + (px - mx) / 2;
		CGFloat fp5y = my - (my - py) / 2;
		CGFloat pm4x = mx - (m / 8); // BUGBUG : fudged
		CGFloat pm4y = my - (m / 6); // BUGBUG : fudged
		CGFloat pm5x = px - (m / 6); // BUGBUG : fudged
		CGFloat pm5y = 0;
	
		_a = CGPointMake(xAdj - kx, yAdj - ky);
		_b = CGPointMake(xAdj - lx, yAdj - ly);
		_c = CGPointMake(xAdj - mx, yAdj - my);
		_d = CGPointMake(xAdj - nx, yAdj - ny);
		_e = CGPointMake(xAdj - ox, yAdj - oy);
		_f = CGPointMake(xAdj - px, yAdj - py);
		_g = CGPointMake(xAdj - ox, yAdj + oy);
		_h = CGPointMake(xAdj - nx, yAdj + ny);
		_i = CGPointMake(xAdj - mx, yAdj + my);
		_j = CGPointMake(xAdj - lx, yAdj + ly);
		_k = CGPointMake(xAdj + kx, yAdj + ky);
		_l = CGPointMake(xAdj + lx, yAdj + ly);
		_m = CGPointMake(xAdj + mx, yAdj + my);
		_n = CGPointMake(xAdj + nx, yAdj + ny);
		_o = CGPointMake(xAdj + ox, yAdj + oy);
		_p = CGPointMake(xAdj + px, yAdj + py);
		_q = CGPointMake(xAdj + ox, yAdj - oy);
		_r = CGPointMake(xAdj + nx, yAdj - ny);
		_s = CGPointMake(xAdj + mx, yAdj - my);
		_t = CGPointMake(xAdj + lx, yAdj - ly);
		_u = CGPointMake(xAdj - wx, yAdj - wy);
		_v = CGPointMake(xAdj - wx, yAdj + wy);
		_w = CGPointMake(xAdj + wx, yAdj + wy);
		_x = CGPointMake(xAdj + wx, yAdj - wy);
		
		_fp1 = CGPointMake(xAdj + kx, yAdj - my);
		_fp2 = CGPointMake(xAdj - fp5x, yAdj - fp5y);
		_fp3 = CGPointMake(xAdj - fp5x, yAdj + fp5y);
		_fp4 = CGPointMake(xAdj + kx, yAdj + my);
		_fp5 = CGPointMake(xAdj + fp5x, yAdj + fp5y);
		_fp6 = CGPointMake(xAdj + fp5x, yAdj - fp5y);
		
		// TODO : These aren't quite right...need to have marker towards the middle of the board
		_playerMarker1 = CGPointMake(xAdj - pm4x, yAdj - pm4y);
		_playerMarker2 = CGPointMake(xAdj - pm5x, xAdj + pm5y);
		_playerMarker3 = CGPointMake(xAdj - pm4x, xAdj + pm4y);
		_playerMarker4 = CGPointMake(xAdj + pm4x, yAdj + pm4y);
		_playerMarker5 = CGPointMake(xAdj + pm5x, yAdj + pm5y);
		_playerMarker6 = CGPointMake(xAdj + pm4x, yAdj - pm4y);
	}
	
	// generate individual touching points
	{
		_spotCtr = 0;
#ifdef DEBUG
		for (int i = 0; i < kPointsArraySize; i++)
		{
			_spotRects[i] = CGRectNull;
		}
		
		_circleSize = 0.0;
#endif
	
		// the first 108 are for moving spots
		// for each end of the mark, we need 9 spots
		// for each inward point, we need 5 spots
		// note that the order we do this matters for the offset into the board 
		// as the points are added to the _spotRects array in order
		CGFloat circleSize = 0.0;
		[self calculateSubPoints:_a point2:_t circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_b point2:_a circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_c point2:_b circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_d point2:_c circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_u point2:_d circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_e point2:_u circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_f point2:_e circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_g point2:_f circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_v point2:_g circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_h point2:_v circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_i point2:_h circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_j point2:_i circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_k point2:_j circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_l point2:_k circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_m point2:_l circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_n point2:_m circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_w point2:_n circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_o point2:_w circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_p point2:_o circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_q point2:_p circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_x point2:_q circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_r point2:_x circleSize:&circleSize withCount:5 skipBothEnds:NO];
		[self calculateSubPoints:_s point2:_r circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_t point2:_s circleSize:&circleSize withCount:6 skipBothEnds:NO];
		
		// next 30 are for final spots
		[self calculateSubPoints:_fp1 point2:_a circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_fp2 point2:_u circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_fp3 point2:_v circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_fp4 point2:_k circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_fp5 point2:_w circleSize:&circleSize withCount:6 skipBothEnds:NO];
		[self calculateSubPoints:_fp6 point2:_x circleSize:&circleSize withCount:6 skipBothEnds:NO];
		
		// next 30 are for home spots
		[self calculateSubPoints:_b point2:_d circleSize:&circleSize withCount:7 skipBothEnds:YES];
		[self calculateSubPoints:_e point2:_g circleSize:&circleSize withCount:7 skipBothEnds:YES];
		[self calculateSubPoints:_h point2:_j circleSize:&circleSize withCount:7 skipBothEnds:YES];
		[self calculateSubPoints:_l point2:_n circleSize:&circleSize withCount:7 skipBothEnds:YES];
		[self calculateSubPoints:_o point2:_q circleSize:&circleSize withCount:7 skipBothEnds:YES];
		[self calculateSubPoints:_r point2:_t circleSize:&circleSize withCount:7 skipBothEnds:YES];
		
#ifdef DEBUG
		// TODO : calculate the space between all points generated so far are the same
#endif
		
	}
}

static inline CGRect MakeMarbleDrawRect(CGRect touchRect);
static inline CGRect MakeMarbleDrawRect(CGRect touchRect)
{
	CGFloat cs = touchRect.size.width;
	CGFloat adjCS = cs * kCircleAdjust;
	return CGRectMake(
		touchRect.origin.x + (cs / 2) - (adjCS / 2),
		touchRect.origin.y + (cs / 2) - (adjCS / 2),
		adjCS,
		adjCS
		);
}

-(void)calculateSubPoints:(CGPoint)point1 point2:(CGPoint)point2 circleSize:(CGFloat*)circleSize withCount:(int)count skipBothEnds:(BOOL)skipBothEnds
{
	CGFloat diffX = ((point1.x - point2.x) / (count - 1));
	CGFloat diffY = ((point1.y - point2.y) / (count - 1));
	
	CGFloat cs = 0.0;
	if (circleSize)
	{
		cs = *circleSize;
	}
	
	if (cs == 0.0)
	{
		cs = diffX;
		if (diffX == 0)
		{
			cs = diffY;
		}
		else if (diffY != 0 && diffY < diffX)
		{
			cs = diffY;
		}
		
		if (cs < 0) cs *= -1;
		if (circleSize) *circleSize = cs;
	}
	
	// we never do the first spot because we assume somebody else did it for us :)
	// since they are all end to end
	for (int i = 1; i < (skipBothEnds ? count - 1 : count); i++)
	{
		dassert(cs > 0);
		CGRect spotRect = CGRectMake(point2.x + (i * diffX) - (cs / 2), point2.y + (i * diffY) - (cs / 2), cs, cs);
		dassert(_spotCtr < kPointsArraySize);
		_spotRects[_spotCtr] = spotRect;
		_spotCtr++;
	}
}

- (void)labelPoint:(CGPoint)point withText:(NSString *)text inContext:(CGContextRef)ctx
{
	[text drawAtPoint:point withAttributes:nil];
}

- (void)drawRect:(CGRect)rect
{
	// TODO : can make this faster if we took rect into account
	CGContextRef ctx = UIGraphicsGetCurrentContext();
	
	CGContextTranslateCTM(ctx, 0, self.bounds.size.height);
	CGContextScaleCTM(ctx, 1, -1);
	
	TGMGame *game = self.game;
	TGMBoard *board = GameGetBoard(game);
	if (board)
	{
		for (int i = 0; i < kFinalPointsStarting; i++)
		{
			CGRect spotRect = _spotRects[i];
			dassert(!CGRectIsNull(spotRect));
			spotRect = MakeMarbleDrawRect(spotRect);
			
			int boardI = (i + kPointsOffsetToBoard + kFinalPointsStarting) % kFinalPointsStarting;
			MarbleColor mc = BoardMarbleColorAtSpot(board, boardI);
			CGContextSetFillColorWithColor(ctx, [self colorForMarbleColor:mc].CGColor);
			CGContextFillEllipseInRect(ctx, spotRect);
		}
		
		for (int i = kFinalPointsStarting; i < kHomePointsStarting; i++)
		{
			CGRect spotRect = _spotRects[i];
			dassert(!CGRectIsNull(spotRect));
			spotRect = MakeMarbleDrawRect(spotRect);
			
			int base = (i - kFinalPointsStarting);
			int player = base / kMarblesPerPlayer;
			int finalSpot = base % kMarblesPerPlayer;
			
			PlayerColor pc = PlayerColorForPosition(player);
			MarbleColor mc = BoardMarbleColorInFinalSpot(board, finalSpot, pc);
			
			CGContextSetFillColorWithColor(ctx, [self colorForMarbleColor:mc].CGColor);
			CGContextFillEllipseInRect(ctx, spotRect);
		}
		
		for (int i = kHomePointsStarting; i < kPointsArraySize; i++)
		{
			CGRect spotRect = _spotRects[i];
			dassert(!CGRectIsNull(spotRect));
			spotRect = MakeMarbleDrawRect(spotRect);
			
			int base = (i - kHomePointsStarting);
			int player = base / kMarblesPerPlayer;
			int marble = base % kMarblesPerPlayer;
			
			PlayerColor pc = PlayerColorForPosition(player);
			MarbleColor mc = GetMarbleForPlayer(pc, marble);
			
			BOOL hasUnusedMarble = PlayerHasUnusedMarbleColor(GamePlayerForColor(game, pc), mc);
			UIColor *color = hasUnusedMarble ? [self colorForPlayerColor:pc] : _holeColor;
			CGContextSetFillColorWithColor(ctx, color.CGColor);
			CGContextFillEllipseInRect(ctx, spotRect);
		}
	}
	
	[self drawHighlightedSpot:_highlightedSpot1 withColor:_highlightColor1 withContext:ctx];
	[self drawHighlightedSpot:_highlightedSpot2 withColor:_highlightColor2 withContext:ctx];
	
	for (TGMTouchSpot *spot in _validTouchSpots)
	{
		int sp = [self spotForTouchSpot:spot];
		if (sp != _highlightedSpot1 && sp != _highlightedSpot2)
		{
			[self drawHighlightedSpot:sp withColor:_highlightColor3 withContext:ctx];
		}
	}
	
#if DRAW_PLAYER_MARKERS
	{
		// do this before since, the marker is smaller than the showing position
		CGContextSetFillColorWithColor(ctx, [UIColor cyanColor].CGColor);
		CGFloat markerOffset = 2 * kHighlightExtraPixels;
		CGContextFillEllipseInRect(ctx, CGRectMake(_playerMarker1.x - markerOffset, _playerMarker1.y - markerOffset, 2 * markerOffset, 2 * markerOffset));
		CGContextFillEllipseInRect(ctx, CGRectMake(_playerMarker2.x - markerOffset, _playerMarker2.y - markerOffset, 2 * markerOffset, 2 * markerOffset));
		CGContextFillEllipseInRect(ctx, CGRectMake(_playerMarker3.x - markerOffset, _playerMarker3.y - markerOffset, 2 * markerOffset, 2 * markerOffset));
		CGContextFillEllipseInRect(ctx, CGRectMake(_playerMarker4.x - markerOffset, _playerMarker4.y - markerOffset, 2 * markerOffset, 2 * markerOffset));
		CGContextFillEllipseInRect(ctx, CGRectMake(_playerMarker5.x - markerOffset, _playerMarker5.y - markerOffset, 2 * markerOffset, 2 * markerOffset));
		CGContextFillEllipseInRect(ctx, CGRectMake(_playerMarker6.x - markerOffset, _playerMarker6.y - markerOffset, 2 * markerOffset, 2 * markerOffset));
	}
#endif

	
	// draw player marker
	TGMPlayer* currentPlayer = GameGetCurrentPlayer(game);
	if (currentPlayer)
	{
		CGPoint cpm = CGPointZero;
		int pos = PositionForPlayerColor(PlayerGetColor(currentPlayer));
		if (pos == 0) cpm = _playerMarker1;
		else if (pos == 1) cpm = _playerMarker2;
		else if (pos == 2) cpm = _playerMarker3;
		else if (pos == 3) cpm = _playerMarker4;
		else if (pos == 4) cpm = _playerMarker5;
		else if (pos == 5) cpm = _playerMarker6;
		else dassert(false);
		// TODO : Not final design
		CGContextSetFillColorWithColor(ctx, [UIColor purpleColor].CGColor);
		
		CGRect spotRect = _spotRects[0];
		dassert(!CGRectIsNull(spotRect));
		
		CGFloat cs = spotRect.size.width;
		CGFloat adjCS = cs * kCircleAdjust;
		
		CGContextFillEllipseInRect(ctx, CGRectMake(cpm.x - kHighlightExtraPixels, cpm.y - kHighlightExtraPixels, adjCS, adjCS));
	}

#if DRAW_LINES
	CGContextSaveGState(ctx);
	CGContextSetRGBStrokeColor(ctx, 0, 0, 0, 1);
	CGContextMoveToPoint   (ctx, _a.x, _a.y);
	CGContextAddLineToPoint(ctx, _b.x, _b.y);
	CGContextAddLineToPoint(ctx, _c.x, _c.y);
	CGContextAddLineToPoint(ctx, _d.x, _d.y);
	CGContextAddLineToPoint(ctx, _u.x, _u.y);
	CGContextAddLineToPoint(ctx, _e.x, _e.y);
	CGContextAddLineToPoint(ctx, _f.x, _f.y);
	CGContextAddLineToPoint(ctx, _g.x, _g.y);
	CGContextAddLineToPoint(ctx, _v.x, _v.y);
	CGContextAddLineToPoint(ctx, _h.x, _h.y);
	CGContextAddLineToPoint(ctx, _i.x, _i.y);
	CGContextAddLineToPoint(ctx, _j.x, _j.y);
	CGContextAddLineToPoint(ctx, _k.x, _k.y);
	CGContextAddLineToPoint(ctx, _l.x, _l.y);
	CGContextAddLineToPoint(ctx, _m.x, _m.y);
	CGContextAddLineToPoint(ctx, _n.x, _n.y);
	CGContextAddLineToPoint(ctx, _w.x, _w.y);
	CGContextAddLineToPoint(ctx, _o.x, _o.y);
	CGContextAddLineToPoint(ctx, _p.x, _p.y);
	CGContextAddLineToPoint(ctx, _q.x, _q.y);
	CGContextAddLineToPoint(ctx, _x.x, _x.y);
	CGContextAddLineToPoint(ctx, _r.x, _r.y);
	CGContextAddLineToPoint(ctx, _s.x, _s.y);
	CGContextAddLineToPoint(ctx, _t.x, _t.y);
	CGContextAddLineToPoint(ctx, _a.x, _a.y);
	CGContextStrokePath(ctx);
	CGContextRestoreGState(ctx);
#endif // DRAW_LINES
	
	
#if DRAW_LETTERS
	[self labelPoint:_a withText:@"A" inContext:ctx];
	[self labelPoint:_b withText:@"B" inContext:ctx];
	[self labelPoint:_c withText:@"C" inContext:ctx];
	[self labelPoint:_d withText:@"D" inContext:ctx];
	[self labelPoint:_e withText:@"E" inContext:ctx];
	[self labelPoint:_f withText:@"F" inContext:ctx];
	[self labelPoint:_g withText:@"G" inContext:ctx];
	[self labelPoint:_h withText:@"H" inContext:ctx];
	[self labelPoint:_i withText:@"I" inContext:ctx];
	[self labelPoint:_j withText:@"J" inContext:ctx];
	[self labelPoint:_k withText:@"K" inContext:ctx];
	[self labelPoint:_l withText:@"L" inContext:ctx];
	[self labelPoint:_m withText:@"M" inContext:ctx];
	[self labelPoint:_n withText:@"N" inContext:ctx];
	[self labelPoint:_o withText:@"O" inContext:ctx];
	[self labelPoint:_p withText:@"P" inContext:ctx];
	[self labelPoint:_q withText:@"Q" inContext:ctx];
	[self labelPoint:_r withText:@"R" inContext:ctx];
	[self labelPoint:_s withText:@"S" inContext:ctx];
	[self labelPoint:_t withText:@"T" inContext:ctx];
	[self labelPoint:_u withText:@"U" inContext:ctx];
	[self labelPoint:_v withText:@"V" inContext:ctx];
	[self labelPoint:_w withText:@"W" inContext:ctx];
	[self labelPoint:_x withText:@"X" inContext:ctx];
	[self labelPoint:_fp1 withText:@"FP1" inContext:ctx];
	[self labelPoint:_fp2 withText:@"FP2" inContext:ctx];
	[self labelPoint:_fp3 withText:@"FP3" inContext:ctx];
	[self labelPoint:_fp4 withText:@"FP4" inContext:ctx];
	[self labelPoint:_fp5 withText:@"FP5" inContext:ctx];
	[self labelPoint:_fp6 withText:@"FP6" inContext:ctx];
	[self labelPoint:_playerMarker1 withText:@"PM1" inContext:ctx];
	[self labelPoint:_playerMarker2 withText:@"PM2" inContext:ctx];
	[self labelPoint:_playerMarker3 withText:@"PM3" inContext:ctx];
	[self labelPoint:_playerMarker4 withText:@"PM4" inContext:ctx];
	[self labelPoint:_playerMarker5 withText:@"PM5" inContext:ctx];
	[self labelPoint:_playerMarker6 withText:@"PM6" inContext:ctx];
	
	
#endif

#if DRAW_NUMBERS
	for (int i = 0; i < kPointsArraySize; i++)
	{
		CGRect spotRect = _spotRects[i];
		if (!CGRectIsNull(spotRect))
		{
			int spotNum = i;
#if DRAW_BOARD_NUMBERS
			if (spotNum < kFinalPointsStarting)
			{
				spotNum += kPointsOffsetToBoard;
				spotNum = WrapSpot(spotNum);
			}
#endif
			[self labelPoint:CGPointMake(spotRect.origin.x, spotRect.origin.y) withText:[NSString stringWithFormat:@"%d", spotNum] inContext:ctx];
		}
	}
#endif
	
#if DRAW_PLAYER_POINTS
	for (PlayerColor pc = Player_Min; pc <= Player_Max; IteratePlayerColor(pc))
	{
		CGPoint playerPoint = [self pointForPlayerColor:pc];
		[self labelPoint:playerPoint withText:@"PT" inContext:ctx];
	}
#endif
}

- (void)drawHighlightedSpot:(int)spot withColor:(UIColor *)color withContext:(CGContextRef)ctx
{
	if (spot != -1)
	{
		dassert(spot < kPointsArraySize);
		CGRect spotRect = _spotRects[spot];
		dassert(!CGRectIsNull(spotRect));
		
		CGRect highRect = CGRectMake(
			spotRect.origin.x - kHighlightExtraPixels,
			spotRect.origin.y - kHighlightExtraPixels,
			spotRect.size.width + 2 * kHighlightExtraPixels,
			spotRect.size.height + 2 * kHighlightExtraPixels);
		CGContextSetFillColorWithColor(ctx, color.CGColor);
		CGContextFillEllipseInRect(ctx, highRect);
	}
}

- (UIColor *)colorForMarbleColor:(MarbleColor)mc
{
	if (mc == Color_None)
		return _holeColor;
	
	PlayerColor pc = PlayerColorForMarbleColor(mc);
	return [self colorForPlayerColor:pc];
}

- (UIColor *)colorForPlayerColor:(PlayerColor)pc
{
	// TODO : it would be much better to have actual images, right? ... or does that make it that much slower?
	switch (pc)
	{
		case Player_Red:
			return _redColor;
		case Player_Yellow:
			return _yellowColor;
		case Player_Green:
			return _greenColor;
		case Player_Blue:
			return _blueColor;
		case Player_White:
			return _whiteColor;
		case Player_Black:
			return _blackColor;
		case Player_None:
			dassert(0);
			return nil;
	}
	
	return nil;
}

- (void)highlightSpot1:(TGMTouchSpot *)spot1
{
	if (spot1 == nil)
	{
		[self clearHighlights];
	}
	else
	{
		_highlightedSpot1 = [self spotForTouchSpot:spot1];
		_highlightedSpot2 = -1;
		[self setNeedsDisplay];
	}
}

- (void)clearHighlights
{
	if (_highlightedSpot1 != -1)
	{
		_highlightedSpot1 = -1;
		[self setNeedsDisplay];
	}

	if (_highlightedSpot2 != -1)
	{
		_highlightedSpot2 = -1;
		[self setNeedsDisplay];
	}
}

- (NSArray<TGMTouchSpot *> *)validTouchSpots
{
	return [_validTouchSpots copy];
}

- (void)setValidTouchSpots:(NSArray<TGMTouchSpot *> *)touchSpots
{
	if (![_validTouchSpots isEqualToArray:touchSpots])
	{
		_validTouchSpots = [touchSpots copy];
		[self setNeedsDisplay];
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	if ([touches count] == 1) // don't support multitouch
	{
		[self processTouch:[touches anyObject]];
	}
	
	[super touchesEnded:touches withEvent:event];
}

- (void)processTouch:(UITouch *)touch
{
	BoardViewEventType type = BoardViewEventType_Unknown;
	int spot = [self spotForTouch:touch];
	if (spot == -1)
		return;
	
	BOOL isValidSpot = YES;
	TGMTouchSpot* touchSpot = [self touchSpotForSpot:spot];
	if (spot == _highlightedSpot2)
	{
		type = BoardViewEventType_Spot2Cleared;
		
		// just clear highlight 2
		if (_highlightedSpot2 != -1)
		{
			_highlightedSpot2 = -1;
			[self setNeedsDisplay];
		}
	}
	else if (spot == _highlightedSpot1)
	{
		type = BoardViewEventType_Spot1Cleared;
		
		// clear all highlights
		[self clearHighlights];
	}
	else
	{
		isValidSpot = NO;
		for (TGMTouchSpot *validSpot in _validTouchSpots)
		{
			if ([validSpot isEqualToSpot:touchSpot])
			{
				isValidSpot = YES;
				break;
			}
		}
		
		if (isValidSpot)
		{
			if (_highlightedSpot1 == -1)
			{
				type = BoardViewEventType_Spot1Added;
				_highlightedSpot1 = spot;
			}
			else if (_highlightedSpot2 == -1)
			{
				type = BoardViewEventType_Spot2Added;
				_highlightedSpot2 = spot;
			}
			else
			{
				type = BoardViewEventType_Spot2Changed;
				_highlightedSpot2 = spot;
			}
			
			[self setNeedsDisplay];
		}
	}

	if (isValidSpot)
	{
		[_delegate boardView:self touchedSpot:touchSpot eventType:type];
	}
}

- (int)maxBufferLevel
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		return 2;
	}
	else
	{
		// phone needs a bit more love especially when zoomed out
		return 4;
	}
}

#define kTouchExtraPixelsLvl0 0.0f
#define kTouchExtraPixelsLvl1 10.0f
#define kTouchExtraPixelsLvl2 20.0f
#define kTouchExtraPixelsLvl3 40.0f
#define kTouchExtraPixelsLvl4 80.0f
-(int)spotForTouch:(UITouch *)touch
{
	CGPoint touchPoint = [touch locationInView:self];
	// change the touch point around to match our expected coordinate system
	touchPoint = CGPointMake(touchPoint.x, self.bounds.size.height - touchPoint.y);

	// we do multiple passes looking for the touch
	// level 0 : no extra buffer
	// level 1 : extra buffer
	// level 2 : even more extra buffer
	for (int lvl = 0; lvl <= [self maxBufferLevel]; lvl++)
	{
		CGFloat extraPixels = 0.0f;
		if (lvl == 0) extraPixels = kTouchExtraPixelsLvl0;
		else if (lvl == 1) extraPixels = kTouchExtraPixelsLvl1;
		else if (lvl == 2) extraPixels = kTouchExtraPixelsLvl2;
		else if (lvl == 3) extraPixels = kTouchExtraPixelsLvl3;
		else if (lvl == 4) extraPixels = kTouchExtraPixelsLvl4;
		else assert(false);
		extraPixels *= [UIScreen mainScreen].scale;
		for (int i = 0; i < kPointsArraySize; i++)
		{
			// adjust spotRect to have bigger touch area
			CGRect spotRect = _spotRects[i];
			spotRect = CGRectMake(
				spotRect.origin.x - extraPixels, 
				spotRect.origin.y - extraPixels, 
				spotRect.size.width + 2 * extraPixels, 
				spotRect.size.height + 2 * extraPixels);
		
			if (CGRectContainsPoint(spotRect, touchPoint))
			{
				return i;
			}
		}
	}
	
	return -1;
}

- (int)spotForTouchSpot:(TGMTouchSpot *)spot
{
	int sp = -1;
	if (spot != nil)
	{	
		if (spot.isFinalSpot)
		{
			sp = kFinalPointsStarting + PositionForPlayerColor(spot.playerColor) * kMarblesPerPlayer + spot.spot;
		}
		else if (spot.isHomeSpot)
		{
			sp = kHomePointsStarting + PositionForPlayerColor(spot.playerColor) * kMarblesPerPlayer + spot.spot;
		}
		else
		{
			sp = (spot.spot - kPointsOffsetToBoard) % kTotalSpots; // inverse way
		}
	}
	
	return sp;
}

- (TGMTouchSpot *)touchSpotForSpot:(int)spot
{
	if (spot == -1)
		return nil;
	
	BOOL isFinalSpot = spot >= kFinalPointsStarting && spot < kHomePointsStarting;
	BOOL isHomeSpot = spot >= kHomePointsStarting;
	PlayerColor pc = Player_None;
	if (isHomeSpot)
	{
		int sp = spot - kHomePointsStarting;
		int player = sp / kMarblesPerPlayer;
		pc = PlayerColorForPosition(player);
		spot = sp % kMarblesPerPlayer;
	}
	else if (isFinalSpot)
	{
		int sp = spot - kFinalPointsStarting;
		int player = sp / kMarblesPerPlayer;
		spot = sp % kMarblesPerPlayer;
		pc = PlayerColorForPosition(player);
		dassert(IsValidFinalSpot(spot));
	}
	else
	{
		spot = (spot + kPointsOffsetToBoard + kFinalPointsStarting) % kFinalPointsStarting;
		dassert(IsValidSpot(spot));
	}
	
	return [TGMTouchSpot touchSpotWithSpot:spot playerColor:pc isFinalSpot:isFinalSpot isHomeSpot:isHomeSpot];
}

-(TGMTouchSpot *)spot1
{
	return [self touchSpotForSpot:_highlightedSpot1];
}

-(TGMTouchSpot *)spot2
{
	return [self touchSpotForSpot:_highlightedSpot2];
}

@end
