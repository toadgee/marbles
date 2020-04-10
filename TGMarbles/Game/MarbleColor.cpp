#include "precomp.h"
#include "DebugAssert.h"
#include "MarbleColor.h"
#include "PlayerColor.h"

MarbleColor GetMarbleForPlayer(PlayerColor color, int i)
{
	dassert(IsPlayerColor(color));
	MarbleColor mc = Color_None;
	bool set = true;
	switch (color)
	{
		case Player_Red:
			mc = Color_Red1;
			break;
		case Player_Yellow:
			mc = Color_Yellow1;
			break;
		case Player_Green:
			mc = Color_Green1;
			break;
		case Player_Blue:
			mc = Color_Blue1;
			break;
		case Player_White:
			mc = Color_White1;
			break;
		case Player_Black:
			mc = Color_Black1;
			break;
		case Player_None:
			set = false;
			dassert(false);
			break;
	}

	if (set)
	{
		mc = static_cast<MarbleColor>(mc + i);
		dassert(IsMarbleColor(mc));
		dassert(PlayerColorForMarbleColor(mc) == color);
	}

	return mc;
}

PlayerColor PlayerColorForMarbleColor(MarbleColor color)
{
	static PlayerColor pcs[30];
	static bool s_inited = false;
	if (!s_inited)
	{
		s_inited = true;

		pcs[0] = Player_Red;
		pcs[1] = Player_Red;
		pcs[2] = Player_Red;
		pcs[3] = Player_Red;
		pcs[4] = Player_Red;

		pcs[5] = Player_Yellow;
		pcs[6] = Player_Yellow;
		pcs[7] = Player_Yellow;
		pcs[8] = Player_Yellow;
		pcs[9] = Player_Yellow;

		pcs[10] = Player_Blue;
		pcs[11] = Player_Blue;
		pcs[12] = Player_Blue;
		pcs[13] = Player_Blue;
		pcs[14] = Player_Blue;

		pcs[15] = Player_Green;
		pcs[16] = Player_Green;
		pcs[17] = Player_Green;
		pcs[18] = Player_Green;
		pcs[19] = Player_Green;

		pcs[20] = Player_White;
		pcs[21] = Player_White;
		pcs[22] = Player_White;
		pcs[23] = Player_White;
		pcs[24] = Player_White;

		pcs[25] = Player_Black;
		pcs[26] = Player_Black;
		pcs[27] = Player_Black;
		pcs[28] = Player_Black;
		pcs[29] = Player_Black;
	}

	return pcs[color - 200];
}

bool IsPlayersMarble(MarbleColor marble, PlayerColor player)
{
	switch (player)
	{
	case Player_None:
		dassert(false);
		break;
	case Player_Red:
		return (marble == Color_Red1 || marble == Color_Red2 || marble == Color_Red3 || marble == Color_Red4 || marble == Color_Red5);
	case Player_Yellow:
		return (marble == Color_Yellow1 || marble == Color_Yellow2 || marble == Color_Yellow3 || marble == Color_Yellow4 || marble == Color_Yellow5);
	case Player_Green:
		return (marble == Color_Green1 || marble == Color_Green2 || marble == Color_Green3 || marble == Color_Green4 || marble == Color_Green5);
	case Player_Blue:
		return (marble == Color_Blue1 || marble == Color_Blue2 || marble == Color_Blue3 || marble == Color_Blue4 || marble == Color_Blue5);
	case Player_White:
		return (marble == Color_White1 || marble == Color_White2 || marble == Color_White3 || marble == Color_White4 || marble == Color_White5);
	case Player_Black:
		return (marble == Color_Black1 || marble == Color_Black2 || marble == Color_Black3 || marble == Color_Black4 || marble == Color_Black5);
	}

	return false;
}


