#pragma once

#include "Board.h"
#include "Marble.h"
#include "Deck.h"
#include "Player.h"
#include "CardList.h"
#include "Game.h"
#include "GameLog.h"
#include "Move.h"
#include "MoveList.h"
#include "PlayerColor.h"
#include "MarbleColor.h"

std::string ComputerPlayerDescription(TGMPlayer* player);
std::string MarbleDescription(TGMMarble *marble);
std::string BoardDescription(TGMBoard *board);
std::string BoardDescriptionWithCustomBreaker(TGMBoard *board, int8_t breaker);
std::string CardListDescription(TGMCardList *cardList, bool detailed);
std::string DeckDescription(TGMDeck *deck);
std::string GameFeedbackStats(TGMGame* game);
std::string ColoredString(PlayerColor color, const char* str);
std::string CardDescription(TGMCard *card, bool shortDescription);
std::string GameLogDescription(TGMGameLog* gameLog);
std::string MoveShortDescription(TGMMove* move, bool withCard);
std::string MoveDescription(TGMMove* move);
std::string MoveListDescription(TGMMoveList *moveList);
std::string PlayerDescription(TGMPlayer* player);
std::string CardSuitDescription(CardSuit suit);
std::string PlayerColorToShortString(PlayerColor color);
const char * PlayerColorToString(PlayerColor color);
std::string MarbleColorToShortString(MarbleColor color);
std::string CardNumberToString(CardNumber card);
std::string MarbleGameStateToString(MarbleGameState state);
std::string CardSuitToString(CardSuit suit);
std::string StrategyToString(MarbleStrategy strategy);
std::string GameStateToString(MarbleGameState state);

