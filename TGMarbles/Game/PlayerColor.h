#pragma once
// teams are every other
typedef enum
{
	Player_Red	  = 100,
	Player_Yellow = 101,
	Player_Blue	  = 102,
	Player_Green  = 103,
	Player_White  = 104,
	Player_Black  = 105,

	Player_None	  =	 99,
	Player_Min	  = 100,
	Player_Max	  = 105
} PlayerColor;

#define IteratePlayerColor(x) (x = (static_cast<PlayerColor>(((int)(x)) + 1)))
#define IsPlayerColor(x) ((x) <= Player_Max && (x) >= Player_Min)
#define IsPlayerColorTeam1(x) ((x) == Player_Red || (x) == Player_White || (x) == Player_Blue)
#define NextTeammateColor(teamColor) ((PlayerColor)((((teamColor) + 2 - Player_Min) % (Player_Max - Player_Min + 1)) + Player_Min))
#define OtherTeamColor(myTeamColor) ((PlayerColor)((((myTeamColor) + 1 - Player_Min) % (Player_Max - Player_Min + 1)) + Player_Min)) // returns a random color from the other team
#define ArePlayersTeammates(pc1, pc2) (((pc1) % 2) == ((pc2) % 2))

static inline PlayerColor NextPlayerColor(PlayerColor color)
{
	int next = (int)color + 1;
	if (next > Player_Max)
		next = Player_Min;
	return (PlayerColor)next;
}

