//
//	CardView.m
//	Marbles
//
//	Created by toddha on 12/17/12.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import "MarblesCommon.h"
#import "CardView.h"
#import "MarblesCommon.h"
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
	for (CardSuit suit = CardSuit_Min; suit <= CardSuit_Max; IterateCardSuit(suit))
	{
		NSString *suffix = @"";
		NSString *suffix2 = @"";
		
		// image has clubs, diamonds, hearts, spades
		switch (suit)
		{
			case CardSuit_Hearts:
				suffix = @"_of_hearts";
				break;
			case CardSuit_Diamonds:
				suffix = @"_of_diamonds";
				break;
			case CardSuit_Clubs:
				suffix = @"_of_clubs";
				break;
			case CardSuit_Spades:
				suffix = @"_of_spades";
				break;
			case CardSuit_Joker:
				suffix = @"";
				break;
			case CardSuit_None:
				dassert(false);
				break;
		}
		
		NSMutableDictionary<NSNumber *, UIImage *> *suitImages = [NSMutableDictionary dictionary];
		for (CardNumber card = CardNumber_Min; card <= CardNumber_Max; IterateCardNumber(card))
		{
			if (suit == CardSuit_Joker && card != CardNumber_Joker) continue;
			if (suit != CardSuit_Joker && card == CardNumber_Joker) continue;
		
			NSString *prefix = @"";
			switch (card)
			{
				case CardNumber_Joker:
					prefix = @"red_joker"; // TODO : there are actually two joker cards...
					break;
				case CardNumber_Ace:
					prefix = @"ace";
					break;
				case CardNumber_2:
					prefix = @"2";
					break;
				case CardNumber_3:
					prefix = @"3";
					break;
				case CardNumber_4:
					prefix = @"4";
					break;
				case CardNumber_5:
					prefix = @"5";
					break;
				case CardNumber_6:
					prefix = @"6";
					break;
				case CardNumber_7:
					prefix = @"7";
					break;
				case CardNumber_8:
					prefix = @"8";
					break;
				case CardNumber_9:
					prefix = @"9";
					break;
				case CardNumber_10:
					prefix = @"10";
					break;
				case CardNumber_Jack:
					prefix = @"jack";
					suffix2 = @"2";
					break;
				case CardNumber_Queen:
					prefix = @"queen";
					suffix2 = @"2";
					break;
				case CardNumber_King:
					prefix = @"king";
					suffix2 = @"2";
					break;
				case CardNumber_None:
					dassert(false);
					break;
			}
			
			NSString *name = [NSString stringWithFormat:@"%@%@%@", prefix, suffix, suffix2];
			UIImage *cardImg = [UIImage imageNamed:name];
			cardImg = [cardImg resizableImageWithCapInsets:UIEdgeInsetsZero resizingMode:UIImageResizingModeStretch];
			[suitImages setObject:cardImg forKey:@(card)];
			
			if (first)
			{
				first = NO;
				s_cardImageDefaultSize = [cardImg size];
			}
		}
		
		[s_cardImages setObject:suitImages forKey:@(suit)];
	}
}

+(UIImage *)imageForCard:(TGMCard *)card
{
	dassert(card != nil);
	NSDictionary *suitImages = [s_cardImages objectForKey:@(CardSuit(card))];
	dassert(suitImages != nil);
	UIImage *image = [suitImages objectForKey:@(CardNumber(card))];
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
