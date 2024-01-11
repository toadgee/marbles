#pragma once

enum class CardSuit: uint16_t
{
	Hearts	= 800,
	Diamonds = 801,
	Clubs	   = 802,
	Spades   = 803,
	Joker	   = 804,

	None	   = 799,
	Min	   = 800,
	Max	   = 804
};

#define IsCardSuit(x) ((x) <= CardSuit::Max && (x) >= CardSuit::Min)
#define IterateCardSuit(x) (x = (static_cast<CardSuit>((static_cast<int>(x)) + 1)))

