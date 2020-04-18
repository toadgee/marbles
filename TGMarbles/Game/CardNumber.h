#pragma once
#define IsNumberedCard(x) ((x) >= CardNumber::Ace && (x) <= CardNumber::King)
#define IsCardNumber(x) ((x) <= CardNumber::Max && (x) >= CardNumber::Min)
#define IterateCardNumber(x) (x = (static_cast<CardNumber>(((int)(x)) + 1)))

enum class CardNumber
{
	Joker       = 700, // must be before Ace
	Ace         = 701,
	Card2       = 702,
	Card3       = 703,
	Card4       = 704,
	Card5       = 705,
	Card6       = 706,
	Card7       = 707,
	Card8       = 708,
	Card9       = 709,
	Card10      = 710,
	Jack        = 711,
	Queen       = 712,
	King        = 713,

	None        = 699,
	Min         = 700,
	MinNumbered = 701,
	Max         = 713
};

int CardMoves(CardNumber card);
