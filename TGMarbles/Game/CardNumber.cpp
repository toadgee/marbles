#include "precomp.h"
#include "CardNumber.h"
#include "DebugAssert.h"

int CardMoves(CardNumber card)
{
	switch (card)
	{
		case CardNumber_Ace:   return 1;
		case CardNumber_2:	   return 2;
		case CardNumber_3:	   return 3;
		case CardNumber_4:	   return -4;
		case CardNumber_5:	   return 5;
		case CardNumber_6:	   return 6;
		case CardNumber_7:	   return 7;
		case CardNumber_8:	   return 8;
		case CardNumber_9:	   return 9;
		case CardNumber_10:	   return 10;
		case CardNumber_Jack:  return 11;
		case CardNumber_Queen: return 12;
		case CardNumber_King:  return 13;
		case CardNumber_Joker:
		case CardNumber_None:
			break;
	}

	dassert(false);
	return 0;
}
