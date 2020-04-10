#pragma once
#include "PlayerColor.h"

typedef enum
{
	// NOTE: PlayerColorForMarbleColor uses these values
	Color_Red1	  = 200,
	Color_Red2	  = 201,
	Color_Red3	  = 202,
	Color_Red4	  = 203,
	Color_Red5	  = 204,

	Color_Yellow1 = 205,
	Color_Yellow2 = 206,
	Color_Yellow3 = 207,
	Color_Yellow4 = 208,
	Color_Yellow5 = 209,

	Color_Blue1	  = 210,
	Color_Blue2	  = 211,
	Color_Blue3	  = 212,
	Color_Blue4	  = 213,
	Color_Blue5	  = 214,

	Color_Green1  = 215,
	Color_Green2  = 216,
	Color_Green3  = 217,
	Color_Green4  = 218,
	Color_Green5  = 219,

	Color_White1  = 220,
	Color_White2  = 221,
	Color_White3  = 222,
	Color_White4  = 223,
	Color_White5  = 224,

	Color_Black1  = 225,
	Color_Black2  = 226,
	Color_Black3  = 227,
	Color_Black4  = 228,
	Color_Black5  = 229,

	Color_None	  = 199, // empty
	Color_Min	  = 199,
	Color_Max	  = 229
} MarbleColor;

#define IsValidMarbleColor(x) ((x) <= Color_Max && (x) >= Color_Min && (x) != Color_None)
#define IsMarbleColor(x) ((x) <= Color_Max && (x) >= Color_Min)

PlayerColor PlayerColorForMarbleColor(MarbleColor color);
MarbleColor GetMarbleForPlayer(PlayerColor color, int i);

bool IsPlayersMarble(MarbleColor marble, PlayerColor player);

