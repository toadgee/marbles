//
//	HandView.m
//	Marbles
//
//	Created by toddha on 2/4/13.
//	Copyright (c) 2013 toadgee.com. All rights reserved.
//

#import "MarblesCommon.h"
#import "HandView.h"
#import "Options.h"

@interface TGMHandView () < TGMCardViewDelegate >
{
	NSArray<TGMCardView *> *_cardViews;
	
	TGMCardView *_selectedCardView;
	TGMCardList *_hand;
}
@end

@implementation TGMHandView

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
	NSMutableArray<TGMCardView *> *cardViews = [NSMutableArray new];
	for (NSUInteger c = 0; c < kCardsPerHand; c++)
	{
		TGMCardView *cv = [[TGMCardView alloc] initWithFrame:CGRectMake(0, 0, 1, 1)];
		[cv setName:[NSString stringWithFormat:@"Hand card %lu", (unsigned long)c]];
		[cv setDelegate:self];
		[cv setUserInteractionEnabled:YES];
		[cardViews addObject:cv];
		[self addSubview:cv];
	}
	
	_cardViews = cardViews;
}

#pragma mark -
#pragma mark Properties
-(TGMCardView *)selectedCardView
{
	return _selectedCardView;
}

-(void)setSelectedCardView:(TGMCardView *)cardView
{
	if (cardView == _selectedCardView) return;
	
	if (cardView == nil || _selectedCardView != nil)
	{
		[_selectedCardView setSelected:NO];
		_selectedCardView = nil;
	}
	
	if (cardView != nil)
	{
		dassert(_selectedCardView == nil);
		
		_selectedCardView = cardView;
		[_selectedCardView setSelected:YES];
	}
	
	[self setNeedsLayout];
	
	[_delegate selectedCardChanged:self];
}

-(TGMCardList *)hand
{
	return _hand;
}

-(void)setHand:(TGMCardList *)hand
{
	if (_hand != hand)
	{
		_hand = hand;
		if (_hand != NULL)
		{
			RetainCardList(hand);
			CardListSort(_hand);
		}
	}
	
	[self setNeedsLayout];
	[self layoutIfNeeded];
	
	if ([self handCount] == 1)
	{
		[self setSelectedCardView:[_cardViews firstObject]];
	}
}

- (NSUInteger)handCount
{
	NSUInteger handCount = _hand == NULL ? 0 : CardListCount(_hand);
	return handCount;
}

#define kCardViewMinimumDisplayedPad 0.5f
#define kCardViewMinimumDisplayedPhone 0.28f
- (CGFloat)cardViewMinimumDisplayed
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		return kCardViewMinimumDisplayedPad;
	}
	else
	{
		return kCardViewMinimumDisplayedPhone;
	}
}

#define kCardViewSelectedPixelsPad 30 // how many pixels to move the card when selected
#define kCardViewSelectedPixelsPhone 20 // how many pixels to move the card when selected
- (CGFloat)cardViewSelectedPixels
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		return kCardViewSelectedPixelsPad;
	}
	else
	{
		return kCardViewSelectedPixelsPhone;
	}
}

#define kHandViewReservedSpace 0.95 // reserved space for help when rotating cards
- (void)layoutSubviews
{
	NSUInteger handCount = [self handCount];
	CGFloat handViewMiddleY = self.frame.size.height / 2;
	
	// adjust the card image size
	CGSize cardImageSize = [TGMCardView cardImageSize];
	CGFloat cardsDisplayed = ((CGFloat)kCardsPerHand - 1) * [self cardViewMinimumDisplayed] + 1;
	CGFloat neededHeight = cardsDisplayed * cardImageSize.height;
	if (cardImageSize.width > (self.frame.size.width * kHandViewReservedSpace))
	{
		CGFloat cw = (CGFloat)(self.frame.size.width * kHandViewReservedSpace);
		CGFloat ch = cw * cardImageSize.height / cardImageSize.width;
		cardImageSize = CGSizeMake(cw, ch);
		neededHeight = cardsDisplayed * cardImageSize.height;
	}
	
	if (self.frame.size.height * kHandViewReservedSpace < neededHeight)
	{
		CGFloat ch = (CGFloat)((self.frame.size.height * kHandViewReservedSpace) / cardsDisplayed);
		CGFloat cw = ch * cardImageSize.width / cardImageSize.height;
		cardImageSize = CGSizeMake(cw, ch);
		neededHeight = cardsDisplayed * cardImageSize.height;
	}
	
	BOOL leftHanded = [TGMOptions sharedInstance].isLeftHanded;
	
	// determine the initial spot for the cards
	CGFloat startingPositionX = leftHanded
		? (CGFloat)(self.frame.size.width - (self.frame.size.width * kHandViewReservedSpace))
		: (CGFloat)((self.frame.size.width * kHandViewReservedSpace) - cardImageSize.width);
	CGFloat startingPositionY = handViewMiddleY - (neededHeight / 2);
	
	if (startingPositionX < 0) startingPositionX = 0;
	if (startingPositionY < 0) startingPositionY = 0;
	
	CGFloat selectedPixels = leftHanded ? [self cardViewSelectedPixels] : 0 - [self cardViewSelectedPixels];
	unsigned hc = 0;
	CGFloat xPos = startingPositionX;
	CGFloat yPos = startingPositionY;
	[UIView beginAnimations:nil context:nil];
	for (TGMCardView *cv in _cardViews)
	{
		TGMCard *card = nil;
		if (handCount > hc)
		{
			card = CardAtIndex(_hand, hc);
		}
		
		BOOL sameCard;
		if (cv.card == nullptr)
		{
			sameCard = (card == nil);
		}
		else if (card == nil)
		{
			sameCard = NO;
		}
		else
		{
			sameCard = (CardUniqueId(card) == CardUniqueId(cv.card));
		}
		
		if (!sameCard)
		{
			[cv setSelected:NO];
		}
		
		cv.card = card;
		if (card != NULL)
		{
			ReleaseCard(card);
		}
		cv.hidden = (card == nil);
		cv.frame = CGRectMake([cv isSelected] ? xPos + selectedPixels : xPos, yPos, cardImageSize.width, cardImageSize.height);
		
		yPos += (cardImageSize.height * [self cardViewMinimumDisplayed]);
		hc++;
	}
	
	[UIView commitAnimations];
}

#pragma mark -
#pragma mark Touch event handlers

-(void)selectedCard:(TGMCardView *)cardView
{
	if ([cardView isSelected] && [self handCount] != 1)
	{
		self.selectedCardView = nil;
	}
	else
	{
		self.selectedCardView = cardView;
	}
	
	[self setNeedsLayout];
}

@end
