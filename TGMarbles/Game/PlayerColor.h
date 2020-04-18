#pragma once
// teams are every other
enum class PlayerColor
{
	Red	 = 100,
	Yellow = 101,
	Blue	 = 102,
	Green  = 103,
	White  = 104,
	Black  = 105,

	None	 =	 99,
	Min	 = 100,
	Max	 = 105
};

#define IteratePlayerColor(x) (x = static_cast<PlayerColor>(static_cast<int>(x) + 1))
#define IsPlayerColor(x) ((x) <= PlayerColor::Max && (x) >= PlayerColor::Min)
#define IsPlayerColorTeam1(x) ((x) == PlayerColor::Red || (x) == PlayerColor::White || (x) == PlayerColor::Blue)
#define ArePlayersTeammates(pc1, pc2) ((static_cast<int>(pc1) % 2) == (static_cast<int>(pc2) % 2))

static inline PlayerColor NextTeammateColor(PlayerColor teamColor)
{
	int tc = static_cast<int>(teamColor);
	int pc = tc + 2 - static_cast<int>(PlayerColor::Min);
	pc %= (static_cast<int>(PlayerColor::Max) - static_cast<int>(PlayerColor::Min) + 1);
	pc += static_cast<int>(PlayerColor::Min);
	return static_cast<PlayerColor>(pc);
}

static inline PlayerColor OtherTeamColor(PlayerColor myTeamColor)
{
	// returns a random color from the other team
	int offset = static_cast<int>(myTeamColor) + 1 - static_cast<int>(PlayerColor::Min);
	offset %= (static_cast<int>(PlayerColor::Max) - static_cast<int>(PlayerColor::Min) + 1);
	offset += static_cast<int>(PlayerColor::Min);
	return static_cast<PlayerColor>(offset);
}

static inline PlayerColor NextPlayerColor(PlayerColor color)
{
	int next = static_cast<int>(color) + 1;
	if (next > static_cast<int>(PlayerColor::Max))
		next = static_cast<int>(PlayerColor::Min);
	return static_cast<PlayerColor>(next);
}

