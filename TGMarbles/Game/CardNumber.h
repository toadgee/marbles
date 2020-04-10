#pragma once
#define IsNumberedCard(x) ((x) >= CardNumber_Ace && (x) <= CardNumber_King)
#define IsCardNumber(x) ((x) <= CardNumber_Max && (x) >= CardNumber_Min)
#define IterateCardNumber(x) (x = (static_cast<CardNumber>(((int)(x)) + 1)))

typedef enum
{
	CardNumber_Joker       = 700, // must be before Ace
	CardNumber_Ace         = 701,
	CardNumber_2           = 702,
	CardNumber_3           = 703,
	CardNumber_4           = 704,
	CardNumber_5           = 705,
	CardNumber_6           = 706,
	CardNumber_7           = 707,
	CardNumber_8           = 708,
	CardNumber_9           = 709,
	CardNumber_10          = 710,
	CardNumber_Jack        = 711,
	CardNumber_Queen       = 712,
	CardNumber_King        = 713,

	CardNumber_None        = 699,
	CardNumber_Min         = 700,
	CardNumber_MinNumbered = 701,
	CardNumber_Max         = 713
} CardNumber;

int CardMoves(CardNumber card);
