#include "precomp.h"
#include "Descriptions.h"
#include "Game.h"
#include "ComputerPlayer.h"
#include "ConsoleUtilities.h"
#include "ConsolePlayer.h"
#include "RandomPlayerColor.h"

void AddPlayersToGame(TGMGame* game, MarbleStrategy team1, MarbleStrategy team2, bool outputHowTheComputerWouldDo);
void DoGame(TGMGame* game, MarbleStrategy team1, MarbleStrategy team2);

void AddPlayersToGame(TGMGame* game, MarbleStrategy team1, MarbleStrategy team2, bool outputHowTheComputerWouldDo)
{
	// generate random colors for the 6 players
	PlayerColor color1;
	PlayerColor color2;
	PlayerColor color3;
	PlayerColor color4;
	PlayerColor color5;
	PlayerColor color6;
	GenerateRandomColors(GameGetRandomNumberGenerator(game), &color1, &color2, &color3, &color4, &color5, &color6);
	
	// team 1
	GameAddPlayer(game, CreateComputerPlayer("Aubrey", team1, color1));
	GameAddPlayer(game, CreateComputerPlayer("Katie",  team1, color3));
	GameAddPlayer(game, CreateComputerPlayer("Shauna", team1, color5));

	// team 2
	GameAddPlayer(game, CreateComputerPlayer("Jason", team2, color2));
	GameAddPlayer(game, CreateComputerPlayer("Ryan",  team2, color4));
	GameAddPlayer(game, CreateConsolePlayer(color6, game, outputHowTheComputerWouldDo));
}

void DoGame(TGMGame* game, MarbleStrategy team1, MarbleStrategy team2)
{
	for (PlayerColor pc = Player_Min; pc <= Player_Max; IteratePlayerColor(pc))
	{
		TGMPlayer* player = GamePlayerForColor(game, pc);
		std::ostringstream str;
		str << PlayerGetName(player);
		str << " - ";
		str << StrategyToString(PlayerGetStrategy(player));
		
		std::string coloredString = ColoredString(PlayerGetColor(player), str.str().c_str());
		printf("%s\n", coloredString.c_str());
	}
	
	// do one simulation here so that we can see if we'd win or lose
	SimulateGame(game, nullptr, true);
	
	// run game
	GameStartNew(game);
}

int main (int argc, const char * argv[])
{
	while (true)
	{
		// TODO : improve reading names, strategies, colors
		MarbleStrategy team1 = Strategy_DefensiveAggressive;
		MarbleStrategy team2 = Strategy_DefensiveAggressive;
		bool onlyTryLosingGames = false;
		
		if (/* DISABLES CODE */ (false))
		{
			for (int i = Strategy_Min; i <= Strategy_Max; i++)
			{
				MarbleStrategy ms = (MarbleStrategy)i;
				if (ms == Strategy_Human) continue;
				printf("%d) %s\n", i, StrategyToString(ms).c_str());
			}
			
			int strat = Strategy_Max + 1;
			while (strat == Strategy_Max + 1)
			{
				strat = ConsoleGetInputNumber("Enter strategy of opposing team", (Strategy_Max + 1), Strategy_Min, Strategy_Max);
				if (strat == Strategy_Human)
				{
					strat = Strategy_Max + 1;
				}
			}
			team1 = static_cast<MarbleStrategy>(strat);
			
			strat = Strategy_Max + 1;
			while (strat == Strategy_Max + 1)
			{
				strat = ConsoleGetInputNumber("Enter strategy of your team", (Strategy_Max + 1), Strategy_Min, Strategy_Max);
				if (strat == Strategy_Human)
				{
					strat = Strategy_Max + 1;
				}
			}
			team2 = static_cast<MarbleStrategy>(strat);
			
			printf("Opposing team is %s\n", StrategyToString(team1).c_str());
			printf("Your team is %s\n", StrategyToString(team2).c_str());
		}
		
		//if (false)
		{
			while (true)
			{
				std::string input { ConsoleGetInput("Would you like to only play games that the computer could not have win if it played for you? (y/N)") };
#if WIN32
				if (_stricmp(input.c_str(), "y") == 0)
#else
				if (strcasecmp(input.c_str(), "y") == 0)
#endif
				{
					onlyTryLosingGames = true;
					break;
				}
#if WIN32
				else if ((_stricmp(input.c_str(), "n") == 0) || (input.size() == 0))
#else
				else if ((strcasecmp(input.c_str(), "n") == 0) || (input.size() == 0))
#endif
				{
					onlyTryLosingGames = false;
					break;
				}
				
				printf("Unknown input\n");
			}
		}
		
#ifdef DEBUG
		TGRandom* rng = CreateRandomWithSeed(101);
#else
		TGRandom* rng = CreateRandomAndSeed();
#endif
		TGMGame* game = CreateGame(NULL, rng);
		AddPlayersToGame(game, team1, team2, onlyTryLosingGames);
		
		bool playerTeamWon = true;
		while (onlyTryLosingGames && playerTeamWon)
		{
			SimulateGame(game, &playerTeamWon, false);
			
			if (playerTeamWon)
			{
				game = CreateGame(NULL, rng);
				AddPlayersToGame(game, team1, team2, onlyTryLosingGames);
			}
		}
		
		printf("Game ID : %u\n\n", RandomSeed(GameGetRandomNumberGenerator(game)));
		DoGame(game, team1, team2);
		
		// TODO : Play again?
		break;
	}
	
	return 0;
}

