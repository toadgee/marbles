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
		case PlayerColor::Red:
			mc = MarbleColor::Red1;
			break;
		case PlayerColor::Yellow:
			mc = MarbleColor::Yellow1;
			break;
		case PlayerColor::Green:
			mc = MarbleColor::Green1;
			break;
		case PlayerColor::Blue:
			mc = MarbleColor::Blue1;
			break;
		case PlayerColor::White:
			mc = MarbleColor::White1;
			break;
		case PlayerColor::Black:
			mc = MarbleColor::Black1;
			break;
		case PlayerColor::None:
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

		pcs[0] = PlayerColor::Red;
		pcs[1] = PlayerColor::Red;
		pcs[2] = PlayerColor::Red;
		pcs[3] = PlayerColor::Red;
		pcs[4] = PlayerColor::Red;

		pcs[5] = PlayerColor::Yellow;
		pcs[6] = PlayerColor::Yellow;
		pcs[7] = PlayerColor::Yellow;
		pcs[8] = PlayerColor::Yellow;
		pcs[9] = PlayerColor::Yellow;

		pcs[10] = PlayerColor::Blue;
		pcs[11] = PlayerColor::Blue;
		pcs[12] = PlayerColor::Blue;
		pcs[13] = PlayerColor::Blue;
		pcs[14] = PlayerColor::Blue;

		pcs[15] = PlayerColor::Green;
		pcs[16] = PlayerColor::Green;
		pcs[17] = PlayerColor::Green;
		pcs[18] = PlayerColor::Green;
		pcs[19] = PlayerColor::Green;

		pcs[20] = PlayerColor::White;
		pcs[21] = PlayerColor::White;
		pcs[22] = PlayerColor::White;
		pcs[23] = PlayerColor::White;
		pcs[24] = PlayerColor::White;

		pcs[25] = PlayerColor::Black;
		pcs[26] = PlayerColor::Black;
		pcs[27] = PlayerColor::Black;
		pcs[28] = PlayerColor::Black;
		pcs[29] = PlayerColor::Black;
	}

	return pcs[static_cast<int>(color) - 200];
}

bool IsPlayersMarble(MarbleColor marble, PlayerColor player)
{
	switch (player)
	{
	case PlayerColor::None:
		dassert(false);
		break;
	case PlayerColor::Red:
		return (marble == MarbleColor::Red1 || marble == MarbleColor::Red2 || marble == MarbleColor::Red3 || marble == MarbleColor::Red4 || marble == MarbleColor::Red5);
	case PlayerColor::Yellow:
		return (marble == MarbleColor::Yellow1 || marble == MarbleColor::Yellow2 || marble == MarbleColor::Yellow3 || marble == MarbleColor::Yellow4 || marble == MarbleColor::Yellow5);
	case PlayerColor::Green:
		return (marble == MarbleColor::Green1 || marble == MarbleColor::Green2 || marble == MarbleColor::Green3 || marble == MarbleColor::Green4 || marble == MarbleColor::Green5);
	case PlayerColor::Blue:
		return (marble == MarbleColor::Blue1 || marble == MarbleColor::Blue2 || marble == MarbleColor::Blue3 || marble == MarbleColor::Blue4 || marble == MarbleColor::Blue5);
	case PlayerColor::White:
		return (marble == MarbleColor::White1 || marble == MarbleColor::White2 || marble == MarbleColor::White3 || marble == MarbleColor::White4 || marble == MarbleColor::White5);
	case PlayerColor::Black:
		return (marble == MarbleColor::Black1 || marble == MarbleColor::Black2 || marble == MarbleColor::Black3 || marble == MarbleColor::Black4 || marble == MarbleColor::Black5);
	}

	return false;
}


