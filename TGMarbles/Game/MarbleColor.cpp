#include "precomp.h"
#include "DebugAssert.h"
#include "MarbleColor.h"
#include "PlayerColor.h"

MarbleColor GetMarbleForPlayer(PlayerColor color, int i)
{
	dassert(IsPlayerColor(color));
	MarbleColor mc = MarbleColor::None;
	bool set = true;
	switch (color)
	{
		case Player_Red:
			mc = MarbleColor::Red1;
			break;
		case Player_Yellow:
			mc = MarbleColor::Yellow1;
			break;
		case Player_Green:
			mc = MarbleColor::Green1;
			break;
		case Player_Blue:
			mc = MarbleColor::Blue1;
			break;
		case Player_White:
			mc = MarbleColor::White1;
			break;
		case Player_Black:
			mc = MarbleColor::Black1;
			break;
		case Player_None:
			set = false;
			dassert(false);
			break;
	}

	if (set)
	{
		mc = static_cast<MarbleColor>(static_cast<int>(mc) + i);
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

	return pcs[static_cast<int>(color) - 200];
}

bool IsPlayersMarble(MarbleColor marble, PlayerColor player)
{
	switch (player)
	{
	case Player_None:
		dassert(false);
		break;
	case Player_Red:
		return (marble == MarbleColor::Red1 || marble == MarbleColor::Red2 || marble == MarbleColor::Red3 || marble == MarbleColor::Red4 || marble == MarbleColor::Red5);
	case Player_Yellow:
		return (marble == MarbleColor::Yellow1 || marble == MarbleColor::Yellow2 || marble == MarbleColor::Yellow3 || marble == MarbleColor::Yellow4 || marble == MarbleColor::Yellow5);
	case Player_Green:
		return (marble == MarbleColor::Green1 || marble == MarbleColor::Green2 || marble == MarbleColor::Green3 || marble == MarbleColor::Green4 || marble == MarbleColor::Green5);
	case Player_Blue:
		return (marble == MarbleColor::Blue1 || marble == MarbleColor::Blue2 || marble == MarbleColor::Blue3 || marble == MarbleColor::Blue4 || marble == MarbleColor::Blue5);
	case Player_White:
		return (marble == MarbleColor::White1 || marble == MarbleColor::White2 || marble == MarbleColor::White3 || marble == MarbleColor::White4 || marble == MarbleColor::White5);
	case Player_Black:
		return (marble == MarbleColor::Black1 || marble == MarbleColor::Black2 || marble == MarbleColor::Black3 || marble == MarbleColor::Black4 || marble == MarbleColor::Black5);
	}

	return false;
}


