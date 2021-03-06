#include "precomp.h"
#include "DebugAssert.h"
#include "Descriptions.h"

// we probably only need color for ship builds
#ifndef DEBUG
#define COLOR_SUPPORTED
#endif

#define FinalSpotChar(spot) (static_cast<char>(SpotToFinalSpot(spot) + 65))
#define BoolToString(b) ((b) ? "YES" : "NO")

#define etstr(save, enumType, stringType) case enumType: save = stringType; break

std::string ColoredString(PlayerColor color, const char* str)
{
	static bool s_use_color_determined = false;
	static bool s_use_color = false;
	if (!s_use_color_determined)
	{
#if WIN32
		size_t size = 0;
		char* term = nullptr;
		errno_t result = _dupenv_s(&term, &size, "TERM");
#else
		const int result = 0;
		const char* term = getenv("TERM");
#endif

		if (result == 0 && term != nullptr)
		{
			printf("%s\n", term);
			if (strcmp(term, "xterm-256color") == 0)
			{
#ifdef COLOR_SUPPORTED
				s_use_color = true;
#endif
			}
#if WIN32
			free(term);
#endif
		}

		s_use_color_determined = true;
	}

	if (!s_use_color)
	{
		return str;
	}

	const char* colorStr = nullptr;
	switch (color)
	{
		case PlayerColor::Black:
			colorStr = "\033[0;35m";// @"\033[1;30m"; // purple hack
			break;
		case PlayerColor::Blue:
			colorStr = "\033[0;34m";
			break;
		case PlayerColor::Green:
			colorStr = "\033[0;32m";
			break;
		case PlayerColor::Red:
			colorStr = "\033[0;31m";
			break;
		case PlayerColor::White:
			colorStr = "\033[0;36m";// @"\033[0;37m"; // cyan hack
			break;
		case PlayerColor::Yellow:
			colorStr = "\033[0;33m";
			break;

		case PlayerColor::None:
			dassert(0);
			break;
	}

	const char* noColor = "\033[0m";

	std::ostringstream ostream;
	ostream << colorStr;
	ostream << str;
	ostream << noColor;
	return ostream.str();
}

std::string BoardDescription(TGMBoard *board)
{
	return BoardDescriptionWithCustomBreaker(board, 6);
}

std::string CardSuitToString(CardSuit suit)
{
	const char* str = nullptr;
	switch (suit)
	{
		etstr(str, CardSuit::None, "None");
		etstr(str, CardSuit::Hearts, "Hearts");
		etstr(str, CardSuit::Diamonds, "Diamonds");
		etstr(str, CardSuit::Clubs, "Clubs");
		etstr(str, CardSuit::Spades, "Spades");
		etstr(str, CardSuit::Max, "MAX");
	}

	return str;
}

std::string CardNumberToString(CardNumber card)
{
	const char* str = nullptr;
	switch (card)
	{
		etstr(str, CardNumber::Joker, "Joker");
		etstr(str, CardNumber::Ace, "Ace");
		etstr(str, CardNumber::Card2, "2");
		etstr(str, CardNumber::Card3, "3");
		etstr(str, CardNumber::Card4, "4");
		etstr(str, CardNumber::Card5, "5");
		etstr(str, CardNumber::Card6, "6");
		etstr(str, CardNumber::Card7, "7");
		etstr(str, CardNumber::Card8, "8");
		etstr(str, CardNumber::Card9, "9");
		etstr(str, CardNumber::Card10, "10");
		etstr(str, CardNumber::Jack, "Jack");
		etstr(str, CardNumber::Queen, "Queen");
		etstr(str, CardNumber::King, "King");
		etstr(str, CardNumber::None, "NONE");
	}

	return str;
}

const char *PlayerColorToString(PlayerColor color)
{
	const char* str = nullptr;
	switch (color)
	{
		etstr(str, PlayerColor::Red, "Red");
		etstr(str, PlayerColor::Yellow, "Yellow");
		etstr(str, PlayerColor::Blue, "Blue");
		etstr(str, PlayerColor::Green, "Green");
		etstr(str, PlayerColor::White, "White");
		etstr(str, PlayerColor::Black, "Black");
		etstr(str, PlayerColor::None, "NONE");
	}

	return str;
}

std::string MarbleColorToShortString(MarbleColor color)
{
	const char* str = nullptr;
	switch (color)
	{
		etstr(str, MarbleColor::None, "_");

		etstr(str, MarbleColor::Red1, "R");
		etstr(str, MarbleColor::Red2, "R");
		etstr(str, MarbleColor::Red3, "R");
		etstr(str, MarbleColor::Red4, "R");
		etstr(str, MarbleColor::Red5, "R");

		etstr(str, MarbleColor::Yellow1, "Y");
		etstr(str, MarbleColor::Yellow2, "Y");
		etstr(str, MarbleColor::Yellow3, "Y");
		etstr(str, MarbleColor::Yellow4, "Y");
		etstr(str, MarbleColor::Yellow5, "Y");

		etstr(str, MarbleColor::Blue1, "L");
		etstr(str, MarbleColor::Blue2, "L");
		etstr(str, MarbleColor::Blue3, "L");
		etstr(str, MarbleColor::Blue4, "L");
		etstr(str, MarbleColor::Blue5, "L");

		etstr(str, MarbleColor::Green1, "G");
		etstr(str, MarbleColor::Green2, "G");
		etstr(str, MarbleColor::Green3, "G");
		etstr(str, MarbleColor::Green4, "G");
		etstr(str, MarbleColor::Green5, "G");

		etstr(str, MarbleColor::White1, "W");
		etstr(str, MarbleColor::White2, "W");
		etstr(str, MarbleColor::White3, "W");
		etstr(str, MarbleColor::White4, "W");
		etstr(str, MarbleColor::White5, "W");

		etstr(str, MarbleColor::Black1, "B");
		etstr(str, MarbleColor::Black2, "B");
		etstr(str, MarbleColor::Black3, "B");
		etstr(str, MarbleColor::Black4, "B");
		etstr(str, MarbleColor::Black5, "B");
	}

	return str;
}

std::string PlayerColorToShortString(PlayerColor color)
{
	const char* str = nullptr;
	switch (color)
	{
		etstr(str, PlayerColor::None, "_");
		etstr(str, PlayerColor::Red, "R");
		etstr(str, PlayerColor::Yellow, "Y");
		etstr(str, PlayerColor::Blue, "L");
		etstr(str, PlayerColor::Green, "G");
		etstr(str, PlayerColor::White, "W");
		etstr(str, PlayerColor::Black, "B");
	}

	return str;
}

std::string StrategyToString(Strategy strategy)
{
	const char* str = nullptr;
	switch (strategy)
	{
		etstr(str, Strategy::None, "NONE");
		etstr(str, Strategy::Human, "Human");
		etstr(str, Strategy::Passive, "Passive");
		etstr(str, Strategy::Aggressive, "Aggressive");
		etstr(str, Strategy::PassiveAggressive, "Passive/Aggressive mix");
		etstr(str, Strategy::Adaptive1, "Passive until near the end of the game");
		etstr(str, Strategy::Adaptive2, "Aggressive until near the end of the game");
		etstr(str, Strategy::DefensivePassive, "Defensive/Passive");
		etstr(str, Strategy::DefensiveAggressive, "Defensive/Aggressive");
	}

	return str;
}

std::string GameStateToString(GameState state)
{
	const char* str = nullptr;
	switch (state)
	{
		etstr(str, GameState::NotStarted, "Not Started");
		etstr(str, GameState::GameStarting, "Game Starting");
		etstr(str, GameState::HandStarting, "Hand Starting");
		etstr(str, GameState::TurnStarting, "Turn Starting");
		etstr(str, GameState::Playing, "Playing");
		etstr(str, GameState::TurnEnding, "Turn Ending");
		etstr(str, GameState::HandEnding, "Hand Ending");
		etstr(str, GameState::GameEnding, "Game Ending");
		etstr(str, GameState::GameOver, "Game Over");
	}

	return str;
}

std::string MarbleDescription(TGMMarble *marble)
{
	std::ostringstream ostream;
	ostream << PlayerColorToString(PlayerColorForMarbleColor(marble->color))
		<< " (distanceFromHome="
		<< marble->distanceFromHome
		<< ", wentBehindHome="
		<< BoolToString(marble->wentBehindHome)
		<< ")";
	return ostream.str();
}

std::string ComputerPlayerDescription(TGMPlayer* player)
{
	std::ostringstream ostream;
	ostream << "["
		<< (PlayerIsTeam1(player) ? 1 : 2)
		<< "] "
		<< PlayerGetName(player)
		<< " ("
		<< PlayerColorToString(PlayerGetColor(player))
		<< ", "
		<< StrategyToString(PlayerGetStrategy(player))
		<< " computer)";
	return ostream.str();
}


std::string PlayerDescription(TGMPlayer* player)
{
	if (player->_onGameStarting != nullptr)
	{
		return player->_onDescription(player);
	}

	std::ostringstream ostream;
	ostream << "["
		<< (PlayerIsTeam1(player) ? 1 : 2)
		<< "] Player ("
		<< StrategyToString(player->_strategy)
		<< ", "
		<< PlayerColorToString(player->_pc)
		<< ")";
	return ostream.str();
}

std::string MoveDescription(TGMMove* move)
{
	std::string spotsString;
	if (move->spotsCalculated)
	{
		std::ostringstream str;
		str << "[";
		int s = 0;
		while (s < kMaxMoveSpots && move->spots[s] != kMoveSpotUnused)
		{
			str << " " << move->spots[s] << " ";
			s++;
		}

		str << "]";
		spotsString = str.str();
	}
	else
	{
		spotsString = "uncalc";
	}

	if (move->isDiscard)
	{
		std::ostringstream str;
		str << "Move ("
			<< PlayerColorToString(move->playerColor)
			<< ") - Discarding "
			<< CardDescription(move->card, false)
			<< " (weight = "
			<< move->weight
			<< ", "
			<< BoolToString(move->weightCalculated)
			<< ", "
			<< spotsString
			<< ")";
		return str.str();
	}

	if (move->oldSpot == kGetOutSpot)
	{
		std::ostringstream str;
		str << "Get "
			<< ColoredString(move->playerColor, "marble")
			<< " out with "
			<< CardDescription(move->card, false)
			<< " (weight = "
			<< move->weight
			<< ", "
			<< BoolToString(move->weightCalculated)
			<< ", "
			<< spotsString
			<< ")";

		return str.str();
	}

	std::string oldSpotStr;
	if (IsFinalSpot(move->oldSpot))
	{
		std::ostringstream str;
		str << static_cast<char>(FinalSpotChar(move->oldSpot));
		oldSpotStr = str.str();
	}
	else
	{
		std::ostringstream str;
		str << move->oldSpot;
		oldSpotStr = str.str();
	}

	std::string newSpotStr;
	if (IsFinalSpot(move->newSpot))
	{
		std::ostringstream str;
		str << static_cast<char>(FinalSpotChar(move->newSpot));
		newSpotStr = str.str();
	}
	else
	{
		std::ostringstream str;
		str << move->newSpot;
		newSpotStr = str.str();
	}

	std::string weightString;
	if (move->weightCalculated)
	{
		std::ostringstream str;
		str << move->weight;
		weightString = str.str();
	}
	else
	{
		weightString = "uncalc";
	}

	std::ostringstream str;
	str << "Move ("
		<< PlayerColorToString(move->playerColor)
		<< ") - ["
		<< move->moves
		<< "] From "
		<< oldSpotStr
		<< " to "
		<< newSpotStr
		<< " with "
		<< CardDescription(move->card, false)
		<< " (weight="
		<< weightString
		<< ", moves="
		<< move->moves
		<< ", jumps="
		<< move->jumps
		<< ", wentBehindHome="
		<< BoolToString(move->wentBehindHome)
		<< ", intermediateSpots="
		<< spotsString
		<< ")";
	return str.str();
}

std::string CardDescription(TGMCard *card, bool shortDescription)
{
	if (card == nullptr) return "[null card]";
	std::string num = CardNumberToString(card->_number);
	if (card->_number == CardNumber::Joker)
	{
		return num;
	}

	std::string suit = CardSuitToString(card->_suit);

	std::ostringstream str;
	str << num << " of " << suit;
	if (!shortDescription)
	{
		str << " (" << card->_uniqueId << ")";
	}

	return str.str();
}

std::string BoardDescriptionWithCustomBreaker(TGMBoard *board, int8_t breaker)
{
	dassert(board != nullptr);
	std::ostringstream str;
	str << "Marbles Board\r\n"; // (intro for lldb)

	// draw all of the final spots from top down
	for (int j = kMarblesPerPlayer - 1; j >= 0; j--)
	{
		for (int i = kTotalSpots - 1; i >= 0; i--)
		{
			if (IsPlayerLastSpot(i))
			{
				PlayerColor color = PlayerColorForLastSpot(i);
				if (board->_finalSpots[PositionForPlayerColor(color)][j] != MarbleColor::None)
				{
					str << ColoredString(color, PlayerColorToShortString(color).c_str());
				}
				else
				{
					str << "_";
				}
			}
			else if (IsPlayerLastSpot(i - 1))
			{
				str << static_cast<char>(FinalSpotChar(j));
			}
			else
			{
				str << " ";
			}

			if ((i + 1) % breaker == 0) str << " ";
		}

		str << "\n";
	}

	// do one pass displaying all marbles
	for (int i = kTotalSpots - 1; i >= 0; i--)
	{
		TGMMarble* marble = board->_board[i];
		MarbleColor mc = marble == nullptr ? MarbleColor::None : marble->color;

		if (mc != MarbleColor::None)
		{
			PlayerColor pc = PlayerColorForMarbleColor(mc);
			str << ColoredString(pc, MarbleColorToShortString(mc).c_str());
		}
		else
		{
			str << MarbleColorToShortString(mc);
		}

		if ((i + 1) % breaker == 0) str << " ";
	}

	// do another pass showing point spots, initial spots, & where final spots are
	str << "\n";
	for (int i = kTotalSpots - 1; i >= 0; i--)
	{
		if (IsPointSpot(i))
		{
			str << "^";
		}
		else if (IsPlayerStartingSpot(i))
		{
			str << "X";
		}
		else if (IsPlayerLastSpot(i))
		{
			PlayerColor c = PlayerColorForLastSpot(i);
			str << ColoredString(c, PlayerColorToShortString(c).c_str());
		}
		else
		{
			str << " ";
		}

		if ((i + 1) % breaker == 0) str << " ";
	}

	// do another 2 passes doing position #s
	str << "\n";
	for (int i = kTotalSpots - 1; i >= 0; i--)
	{
		str << std::hex << (i / 10);
		if ((i + 1) % breaker == 0) str << " ";
	}

	str << "\n";
	for (int i = kTotalSpots - 1; i >= 0; i--)
	{
		str << (i % 10);
		if ((i + 1) % breaker == 0) str << " ";
	}

	// do another pass outputting marbles we haven't played yet
	str << "\n";
	PlayerColor currentSectionColor = PlayerColor::None;
	int distanceToUnusedMarblesStartingSpot = 0;
	int unusedMarblesCount = 0;
	for (int i = kTotalSpots - 1; i >= 0; i--)
	{
		// check if we're in a new section
		if ((i + 1) % kPlayerSpots == 0)
		{
			// if so figure out the current section color
			currentSectionColor = PlayerColorForLastSpot((i + 1) - kPlayerSpots);

			// figure out how many marbles we have
			unusedMarblesCount = SlowInternalGetUnusedMarbleColorCountForPlayer(board, currentSectionColor);

			// figure out the right starting spot for the marbles
			distanceToUnusedMarblesStartingSpot = kPlayerPointPosition - (unusedMarblesCount + 1) / 2;
		}

		if (distanceToUnusedMarblesStartingSpot == 0 && unusedMarblesCount > 0)
		{
			str << ColoredString(currentSectionColor, PlayerColorToShortString(currentSectionColor).c_str());
			unusedMarblesCount--;
		}
		else
		{
			distanceToUnusedMarblesStartingSpot--;
			str << " ";
		}

		if ((i + 1) % breaker == 0) str << " ";
	}

	return str.str();
}

std::string MoveShortDescription(TGMMove* move, bool withCard)
{
	std::ostringstream str;
	if (move->isDiscard)
	{
		str << "Discarding " << CardDescription(move->card, true);
		return str.str();
	}

	if (move->oldSpot == kGetOutSpot)
	{
		str << "Get " << ColoredString(move->playerColor, "marble") << " out";
		if (withCard)
		{
			str << " with " << CardDescription(move->card, true);
		}
		
		return str.str();
	}

	str << ColoredString(move->playerColor, "from") << " ";
	
	if (IsFinalSpot(move->oldSpot))
	{
		str << static_cast<char>(FinalSpotChar(move->oldSpot));
	}
	else
	{
		str <<  move->oldSpot;
	}
	
	str << " to ";
	
	if (IsFinalSpot(move->newSpot))
	{
		str << static_cast<char>(FinalSpotChar(move->newSpot));
	}
	else
	{
		str << move->newSpot;
	}
	
	if (withCard)
	{
		str << " with " << CardDescription(move->card, true);
	}
	
	return str.str();
}

std::string CardListDescription(TGMCardList *cardList, bool detailed)
{
	std::ostringstream str;
	if (detailed)
	{
		str << cardList->_count << " cards";
		if (cardList->_count > 0)
		{
			str << " :\n";
			CardListIterateWithBlock(cardList, [&str](unsigned /*i*/, TGMCard *c)
			{
				str << CardDescription(c, true);
				return true;
			});
		}
	
		return str.str();
	}

	str << "[\n";
	CardListIterateWithBlock(cardList, [&str](unsigned /*i*/, TGMCard *card)
	{
		str << CardDescription(card, false) << ",\n";
		return true;
	});
	
	str << "]";
	
	return str.str();
}

std::string DeckDescription(TGMDeck *deck)
{
	std::ostringstream str;
	str << "Marbles Deck\n";
	str << "\t" << CardListCount(deck->_cards) << " Cards\n";
	CardListIterateWithBlock(deck->_cards, [&str](unsigned /*i*/, TGMCard *card)
	{
		str << "\t" << CardDescription(card, false) << "\n";
		return true;
	});

	str << "\n\t" << CardListCount(deck->_discarded) << " discarded\n";
	CardListIterateWithBlock(deck->_discarded, [&str](unsigned /*i*/, TGMCard *card)
	{
		str << "\t" << CardDescription(card, false) << "\n";
		return true;
	});

	return str.str();
}

std::string GameFeedbackStats(TGMGame* game)
{
	std::ostringstream str;
	str << "score          = " << GameGetTeam1Score(game) << " to " << GameGetTeam2Score(game) << "\n";
	str << "state          = " << GameStateToString(game->_state) << "\n";
	str << "turn number    = " << game->_turn << "\n";
	str << "dealer player  = " << PlayerColorToString(game->_dealingPlayer) << "\n";
	str << "current player = " << PlayerColorToString(game->_currentPlayer) << "\n";
	for (unsigned p = 0; p < kPlayers; p++)
	{
		TGMPlayer* pl = game->_players[p];
		str << "player " << p << " = " << PlayerDescription(pl) << "\n";
	}
	str << "random seed    = " << RandomSeed(game->_rng) << "\n";
	str << "random calls   = " << RandomCalls(game->_rng) << "\n";
	str << BoardDescription(game->_board) << "\n";
	str << DeckDescription(game->_deck) << "\n";
	
	return str.str();
}

std::string GameLogDescription(TGMGameLog* gameLog)
{
	std::ostringstream str;
	str << "GAME LOG\n";
	str << "Dealing player : " << PlayerColorToString(gameLog->_dealingPlayer) << "\n";
	for (uint16_t p = 0; p < kPlayers; p++)
	{
		str << "Player strategy " << PlayerColorToString(PlayerColorForPosition(static_cast<int>(p))) << ": " << StrategyToString(gameLog->_playerStrategy[p]) << "\n";
	}

	// TODO : add DeckListDescription to decklist
	str << "Deck count : " << DeckListCount(gameLog->_deckList) << "\n";
	DeckListIterateWithBlock(gameLog->_deckList, [str2 = &str](unsigned d, TGMDeck* deck) mutable
	{
		(*str2) << "\n\nDeck #" << d << "\n" << DeckDescription(deck) << "\n";
		return true;
	});

	str << "Move count : %u" << MoveListCount(gameLog->_moveList);
	int i = 0;
	TGMMove *move = gameLog->_moveList->first;
	while (move != nullptr)
	{
		str << "\nMove #" << i++ << "    ";
		str << MoveDescription(move);
		move = move->nextMove;
	}

	return str.str();
}

std::string MoveListDescription(TGMMoveList *moveList)
{
	std::ostringstream str;
	TGMMove *m = moveList->first;
	while (m != nullptr)
	{
		str << MoveDescription(m) << "\n";
		m = m->nextMove;
	}
	
	return str.str();
}
