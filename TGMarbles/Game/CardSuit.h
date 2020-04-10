#pragma once

typedef enum
{
	CardSuit_Hearts	  = 800,
	CardSuit_Diamonds = 801,
	CardSuit_Clubs	  = 802,
	CardSuit_Spades	  = 803,
	CardSuit_Joker	  = 804,

	CardSuit_None	  = 799,
	CardSuit_Min	  = 800,
	CardSuit_Max	  = 804
} CardSuit;

#define IsCardSuit(x) ((x) <= CardSuit_Max && (x) >= CardSuit_Min)
#define IterateCardSuit(x) (x = (static_cast<CardSuit>(((int)(x)) + 1)))

