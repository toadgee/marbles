#include "precomp.h"
#include "CardNumber.h"
#include "DebugAssert.h"

int CardMoves(CardNumber card)
{
	switch (card)
	{
		case CardNumber::Ace:   return 1;
		case CardNumber::Card2:	   return 2;
		case CardNumber::Card3:	   return 3;
		case CardNumber::Card4:	   return -4;
		case CardNumber::Card5:	   return 5;
		case CardNumber::Card6:	   return 6;
		case CardNumber::Card7:	   return 7;
		case CardNumber::Card8:	   return 8;
		case CardNumber::Card9:	   return 9;
		case CardNumber::Card10:	   return 10;
		case CardNumber::Jack:  return 11;
		case CardNumber::Queen: return 12;
		case CardNumber::King:  return 13;
		case CardNumber::Joker:
		case CardNumber::None:
			break;
	}

	dassert(false);
	return 0;
}
