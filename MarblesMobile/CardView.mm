//
//	CardView.m
//	Marbles
//
//	Created by toddha on 12/17/12.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import "precomp.h"
#import "CardView.h"
#import "DebugAssert.h"
#import "UIImageExtensions.h"

@interface TGMCardView ()
{
	TGMCard *_card;
}
@end

@implementation TGMCardView

static NSMutableDictionary<NSNumber *, NSDictionary<NSNumber *, UIImage *> *> *s_cardImages = nil;
static CGSize s_cardImageDefaultSize;

+(void)initialize
{
	assert(s_cardImages == nil);
	BOOL first = YES;
	s_cardImages = [NSMutableDictionary new];
	for (CardSuit suit = CardSuit::Min; suit <= CardSuit::Max; IterateCardSuit(suit))
	{
		NSString *suffix = @"";
		NSString *suffix2 = @"";
		
		// image has clubs, diamonds, hearts, spades
		switch (suit)
		{
			case CardSuit::Hearts:
				suffix = @"_of_hearts";
				break;
			case CardSuit::Diamonds:
				suffix = @"_of_diamonds";
				break;
			case CardSuit::Clubs:
				suffix = @"_of_clubs";
				break;
			case CardSuit::Spades:
				suffix = @"_of_spades";
				break;
			case CardSuit::Joker:
				suffix = @"";
				break;
			case CardSuit::None:
				dassert(false);
				break;
		}
		
		NSMutableDictionary<NSNumber *, UIImage *> *suitImages = [NSMutableDictionary dictionary];
		for (CardNumber card = CardNumber::Min; card <= CardNumber::Max; IterateCardNumber(card))
		{
			if (suit == CardSuit::Joker && card != CardNumber::Joker) continue;
			if (suit != CardSuit::Joker && card == CardNumber::Joker) continue;
		
			NSString *prefix = @"";
			switch (card)
			{
				case CardNumber::Joker:
					prefix = @"red_joker"; // TODO : there are actually two joker cards...
					break;
				case CardNumber::Ace:
					prefix = @"ace";
					break;
				case CardNumber::Card2:
					prefix = @"2";
					break;
				case CardNumber::Card3:
					prefix = @"3";
					break;
				case CardNumber::Card4:
					prefix = @"4";
					break;
				case CardNumber::Card5:
					prefix = @"5";
					break;
				case CardNumber::Card6:
					prefix = @"6";
					break;
				case CardNumber::Card7:
					prefix = @"7";
					break;
				case CardNumber::Card8:
					prefix = @"8";
					break;
				case CardNumber::Card9:
					prefix = @"9";
					break;
				case CardNumber::Card10:
					prefix = @"10";
					break;
				case CardNumber::Jack:
					prefix = @"jack";
					suffix2 = @"2";
					break;
				case CardNumber::Queen:
					prefix = @"queen";
					suffix2 = @"2";
					break;
				case CardNumber::King:
					prefix = @"king";
					suffix2 = @"2";
					break;
				case CardNumber::None:
					dassert(false);
					break;
			}
			
			NSString *name = [NSString stringWithFormat:@"%@%@%@", prefix, suffix, suffix2];
			UIImage *cardImg = [UIImage imageNamed:name];
			cardImg = [cardImg resizableImageWithCapInsets:UIEdgeInsetsZero resizingMode:UIImageResizingModeStretch];
			[suitImages setObject:cardImg forKey:@(static_cast<int>(card))];
			
			if (first)
			{
				first = NO;
				s_cardImageDefaultSize = [cardImg size];
			}
		}
		
		[s_cardImages setObject:suitImages forKey:@(static_cast<int>(suit))];
	}
}

+(UIImage *)imageForCard:(TGMCard *)card
{
	dassert(card != nil);
	NSDictionary *suitImages = [s_cardImages objectForKey:@(static_cast<int>(CardSuit(card)))];
	dassert(suitImages != nil);
	UIImage *image = [suitImages objectForKey:@(static_cast<int>(CardNumber(card)))];
	dassert(image != nil);
	return [image copy];
}

+(CGSize)cardImageSize
{
	// can't return the sizes of the images in the dictionary because those might change when asked
	return s_cardImageDefaultSize;
}

- (void)setCard:(TGMCard *)card
{
	if (_card == card)
	{
		return;
	}
	
	_card = card; // TODO : pretty sure this is broken & we need to retain/release here
	
	[self setContentMode:UIViewContentModeScaleAspectFit];
	UIImage *image = nil;
	if (card != nil)
	{
		image = [TGMCardView imageForCard:card];
	}

	[self setBackgroundColor:[UIColor clearColor]];
	[self setImage:image];
}

- (TGMCard *)card
{
	return _card;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	[_delegate selectedCard:self];
	[super touchesEnded:touches withEvent:event];
}

- (NSString *)description
{
	return [NSString stringWithFormat:@"%@ - %@", [self name], [super description]];
}

@end
