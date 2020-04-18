#pragma once
#include "PlayerColor.h"

enum class MarbleColor
{
	// NOTE: PlayerColorForMarbleColor uses these values
	Red1	  = 200,
	Red2	  = 201,
	Red3	  = 202,
	Red4	  = 203,
	Red5	  = 204,

	Yellow1 = 205,
	Yellow2 = 206,
	Yellow3 = 207,
	Yellow4 = 208,
	Yellow5 = 209,

	Blue1	  = 210,
	Blue2	  = 211,
	Blue3	  = 212,
	Blue4	  = 213,
	Blue5	  = 214,

	Green1  = 215,
	Green2  = 216,
	Green3  = 217,
	Green4  = 218,
	Green5  = 219,

	White1  = 220,
	White2  = 221,
	White3  = 222,
	White4  = 223,
	White5  = 224,

	Black1  = 225,
	Black2  = 226,
	Black3  = 227,
	Black4  = 228,
	Black5  = 229,

	None	  = 199, // empty
	Min	  = 199,
	Max	  = 229
};

#define IsValidMarbleColor(x) ((x) <= MarbleColor::Max && (x) >= MarbleColor::Min && (x) != MarbleColor::None)
#define IsMarbleColor(x) ((x) <= MarbleColor::Max && (x) >= MarbleColor::Min)

PlayerColor PlayerColorForMarbleColor(MarbleColor color);
MarbleColor GetMarbleForPlayer(PlayerColor color, int i);

bool IsPlayersMarble(MarbleColor marble, PlayerColor player);

