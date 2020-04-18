#include "precomp.h"
#include "DebugAssert.h"
#include "RandomPlayerColor.h"

void GenerateRandomColors(TGRandom* rng, PlayerColor* color1, PlayerColor* color2, PlayerColor* color3, PlayerColor* color4, PlayerColor* color5, PlayerColor* color6)
{
	// generate random colors for the 6 players
	PlayerColor c1 = static_cast<PlayerColor>((RandomRandom(rng) % (static_cast<int>(PlayerColor::Max) - static_cast<int>(PlayerColor::Min))) + static_cast<int>(PlayerColor::Min)); // GenerateRandomColors
	PlayerColor c2 = NextPlayerColor(c1);
	PlayerColor c3 = NextPlayerColor(c2);
	PlayerColor c4 = NextPlayerColor(c3);
	PlayerColor c5 = NextPlayerColor(c4);
	PlayerColor c6 = NextPlayerColor(c5);
	dassert(IsPlayerColor(c1));
	dassert(IsPlayerColor(c2));
	dassert(IsPlayerColor(c3));
	dassert(IsPlayerColor(c4));
	dassert(IsPlayerColor(c5));
	dassert(IsPlayerColor(c6));
	dassert(c1 != c2 && c1 != c3 && c1 != c4 && c1 != c5 && c1 != c6);
	dassert(c2 != c3 && c2 != c4 && c2 != c5 && c2 != c6);
	dassert(c3 != c4 && c3 != c5 && c3 != c6);
	dassert(c4 != c5 && c4 != c6);
	dassert(c5 != c6);

	if (color1) *color1 = c1;
	if (color2) *color2 = c2;
	if (color3) *color3 = c3;
	if (color4) *color4 = c4;
	if (color5) *color5 = c5;
	if (color6) *color6 = c6;
}

