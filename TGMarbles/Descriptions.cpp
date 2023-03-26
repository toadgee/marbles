#include "precomp.h"
#include "DebugAssert.h"
#include "Descriptions.h"

// we probably only need color for ship builds
#ifndef DEBUG
#define COLOR_SUPPORTED
#endif

inline char FinalSpotChar(int spot)
{
	return static_cast<char>(SpotToFinalSpot(spot) + 'A');
}

inline const char *BoolToString(bool b)
{
	return b ? "YES" : "NO";
}

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
		case CardSuit::None: str = "None"; break;
		case CardSuit::Hearts: str = "Hearts"; break;
		case CardSuit::Diamonds: str = "Diamonds"; break;
		case CardSuit::Clubs: str = "Clubs"; break;
		case CardSuit::Spades: str = "Spades"; break;
		case CardSuit::Max: str = "MAX"; break;
	}

	return str;
}

std::string CardNumberToString(CardNumber card)
{
	const char* str = nullptr;
	switch (card)
	{
		case CardNumber::Joker: str = "Joker"; break;
		case CardNumber::Ace: str = "Ace"; break;
		case CardNumber::Card2: str = "2"; break;
		case CardNumber::Card3: str = "3"; break;
		case CardNumber::Card4: str = "4"; break;
		case CardNumber::Card5: str = "5"; break;
		case CardNumber::Card6: str = "6"; break;
		case CardNumber::Card7: str = "7"; break;
		case CardNumber::Card8: str = "8"; break;
		case CardNumber::Card9: str = "9"; break;
		case CardNumber::Card10: str = "10"; break;
		case CardNumber::Jack: str = "Jack"; break;
		case CardNumber::Queen: str = "Queen"; break;
		case CardNumber::King: str = "King"; break;
		case CardNumber::None: str = "NONE"; break;
	}

	return str;
}

const char *PlayerColorToString(PlayerColor color)
{
	const char* str = nullptr;
	switch (color)
	{
		case PlayerColor::Red: str= "Red"; break;
		case PlayerColor::Yellow: str= "Yellow"; break;
		case PlayerColor::Blue: str= "Blue"; break;
		case PlayerColor::Green: str= "Green"; break;
		case PlayerColor::White: str= "White"; break;
		case PlayerColor::Black: str= "Black"; break;
		case PlayerColor::None: str= "NONE"; break;
	}

	return str;
}

std::string MarbleColorToShortString(MarbleColor color)
{
	const char* str = nullptr;
	switch (color)
	{
		case MarbleColor::None: str = "_"; break;

		case MarbleColor::Red1: str = "R"; break;
		case MarbleColor::Red2: str = "R"; break;
		case MarbleColor::Red3: str = "R"; break;
		case MarbleColor::Red4: str = "R"; break;
		case MarbleColor::Red5: str = "R"; break;

		case MarbleColor::Yellow1: str = "Y"; break;
		case MarbleColor::Yellow2: str = "Y"; break;
		case MarbleColor::Yellow3: str = "Y"; break;
		case MarbleColor::Yellow4: str = "Y"; break;
		case MarbleColor::Yellow5: str = "Y"; break;

		case MarbleColor::Blue1: str = "L"; break;
		case MarbleColor::Blue2: str = "L"; break;
		case MarbleColor::Blue3: str = "L"; break;
		case MarbleColor::Blue4: str = "L"; break;
		case MarbleColor::Blue5: str = "L"; break;

		case MarbleColor::Green1: str = "G"; break;
		case MarbleColor::Green2: str = "G"; break;
		case MarbleColor::Green3: str = "G"; break;
		case MarbleColor::Green4: str = "G"; break;
		case MarbleColor::Green5: str = "G"; break;

		case MarbleColor::White1: str = "W"; break;
		case MarbleColor::White2: str = "W"; break;
		case MarbleColor::White3: str = "W"; break;
		case MarbleColor::White4: str = "W"; break;
		case MarbleColor::White5: str = "W"; break;

		case MarbleColor::Black1: str = "B"; break;
		case MarbleColor::Black2: str = "B"; break;
		case MarbleColor::Black3: str = "B"; break;
		case MarbleColor::Black4: str = "B"; break;
		case MarbleColor::Black5: str = "B"; break;
	}

	return str;
}

std::string PlayerColorToShortString(PlayerColor color)
{
	const char* str = nullptr;
	switch (color)
	{
		case PlayerColor::None: str = "_"; break;
		case PlayerColor::Red: str = "R"; break;
		case PlayerColor::Yellow: str = "Y"; break;
		case PlayerColor::Blue: str = "L"; break;
		case PlayerColor::Green: str = "G"; break;
		case PlayerColor::White: str = "W"; break;
		case PlayerColor::Black: str = "B"; break;
	}

	return str;
}

std::string StrategyToString(Strategy strategy)
{
	const char* str = nullptr;
	switch (strategy)
	{
		case Strategy::None: str = "NONE"; break;
		case Strategy::Human: str = "Human"; break;
		case Strategy::Passive: str = "Passive"; break;
		case Strategy::Aggressive: str = "Aggressive"; break;
		case Strategy::PassiveAggressive: str = "Passive/Aggressive mix"; break;
		case Strategy::Adaptive1: str = "Passive until near the end of the game"; break;
		case Strategy::Adaptive2: str = "Aggressive until near the end of the game"; break;
		case Strategy::DefensivePassive: str = "Defensive/Passive"; break;
		case Strategy::DefensiveAggressive: str = "Defensive/Aggressive"; break;
	}

	return str;
}

std::string GameStateToString(GameState state)
{
	const char* str = nullptr;
	switch (state)
	{
		case GameState::NotStarted: str = "Not Started"; break;
		case GameState::GameStarting: str = "Game Starting"; break;
		case GameState::HandStarting: str = "Hand Starting"; break;
		case GameState::TurnStarting: str = "Turn Starting"; break;
		case GameState::Playing: str = "Playing"; break;
		case GameState::TurnEnding: str = "Turn Ending"; break;
		case GameState::HandEnding: str = "Hand Ending"; break;
		case GameState::GameEnding: str = "Game Ending"; break;
		case GameState::GameOver: str = "Game Over"; break;
	}

	return str;
}

std::string MarbleDescription(TGMMarbleStruct *marble)
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

std::string ComputerPlayerDescription(TGMPlayerStruct* player)
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


std::string PlayerDescription(TGMPlayerStruct* player)
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

std::string MoveDescription(TGMMoveStruct* move)
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
		str << "Move (";
		str << PlayerColorToString(move->playerColor);
		str << ") - Discarding ";
		str << CardDescription(move->card, false);
		str << " (weight = ";
		str << move->weight;
		str << ", ";
		str << BoolToString(move->weightCalculated);
		str << ", ";
		str << spotsString;
		str << ")";
		return str.str();
	}

	if (move->oldSpot == kGetOutSpot)
	{
		std::ostringstream str;
		str << "Get ";
		str << ColoredString(move->playerColor, "marble");
		str << " out with ";
		str << CardDescription(move->card, false);
		str << " (weight = ";
		str << move->weight;
		str << ", ";
		str << BoolToString(move->weightCalculated);
		str << ", ";
		str << spotsString;
		str << ")";

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
	str << "Move (";
	str << PlayerColorToString(move->playerColor);
	str << ") - [";
	str << move->moves;
	str << "] From ";
	str << oldSpotStr;
	str << " to ";
	str << newSpotStr;
	str << " with ";
	str << CardDescription(move->card, false);
	str << " (weight=";
	str << weightString;
	str << ", moves=";
	str << move->moves;
	str << ", jumps=";
	str << move->jumps;
	str << ", wentBehindHome=";
	str << BoolToString(move->wentBehindHome);
	str << ", intermediateSpots=";
	str << spotsString;
	str << ")";
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
