//
//	Player.h
//	Marbles
//
//	Created by todd harris on 11/27/11.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#pragma once
#include "Card.h"
#include "CardList.h"
#include "Marble.h"
#include "MarbleStrategy.h"
#include "PlayerColor.h"

#define kPlayers				6 // number of players

struct TGMPlayerStruct;
struct TGMGameStruct;

typedef void (*TGMPlayerGameStartingFunction)(struct TGMPlayerStruct* player);
typedef void (*TGMPlayerGameEndingFunction)(struct TGMPlayerStruct* player);
typedef void (*TGMPlayerHandStartingFunction)(struct TGMPlayerStruct* player);
typedef void (*TGMPlayerDealtCardFunction)(struct TGMPlayerStruct* player, TGMCard* card);
typedef void (*TGMPlayerPlayInGameFunction)(struct TGMPlayerStruct* player);
typedef void (*TGMPlayerDestroyFunction)(struct TGMPlayerStruct* player);
typedef std::string (*TGMPlayerDescriptionFunction)(struct TGMPlayerStruct* player);


#define TGMPlayer struct TGMPlayerStruct
struct TGMPlayerStruct
{
	int32_t _retainCount;
	void* _holder;
	
	void* _context;
	
	const char* _name;
	bool _marblesInHome[kMarblesPerPlayer];
	TGMCardList* _hand;
	PlayerColor _pc;
	bool _isTeam1;
	MarbleStrategy _strategy;
	struct TGMGameStruct *_game;
	
	TGMPlayerGameStartingFunction _onGameStarting;
	TGMPlayerGameEndingFunction _onGameEnding;
	TGMPlayerHandStartingFunction _onHandStarting;
	TGMPlayerDealtCardFunction _onDealtCard;
	TGMPlayerPlayInGameFunction _onPlayInGame;
	TGMPlayerDescriptionFunction _onDescription;
	TGMPlayerDestroyFunction _onDestroy;
};

TGMPlayer* RetainPlayer(TGMPlayer* player);
void ReleasePlayer(TGMPlayer* player);

TGMPlayer* CreatePlayer(const char* name, MarbleStrategy strategy, PlayerColor pc);
void PlayerCopyData(TGMPlayer* oldPlayer, TGMPlayer* newPlayer);

void PlayerSetName(TGMPlayer* player, const char* name);
void PlayerSetIsTeam1(TGMPlayer* player, bool isTeam1);
void PlayerSetContext(TGMPlayer* player, void* context);

struct TGMGameStruct* PlayerGetGame(TGMPlayer* player);
void PlayerSetGame(TGMPlayer* player, struct TGMGameStruct* game);

int8_t PlayerUnusedMarbleCount(TGMPlayer* player);
bool PlayerHasUnusedMarbleColor(TGMPlayer* player, MarbleColor mc);
MarbleColor PlayerRemoveFirstUnusedMarbleColor(TGMPlayer* player);
void PlayerRestoreMarble(TGMPlayer* player, MarbleColor mc);

void PlayerGameStarting(TGMPlayer* player, struct TGMGameStruct* game);
void PlayerGameEnding(TGMPlayer* player, struct TGMGameStruct* game);
void PlayerStartingHand(TGMPlayer* player);
void PlayerDealtCard(TGMPlayer* player, TGMCard* card);

void PlayerPlayInGame(TGMPlayer *player);

void PlayerSetOnGameStarting(TGMPlayer* player, TGMPlayerGameStartingFunction onGameStarting);
void PlayerSetOnGameEnding(TGMPlayer* player, TGMPlayerGameEndingFunction onGameEnding);
void PlayerSetOnHandStarting(TGMPlayer* player, TGMPlayerHandStartingFunction onHandStarting);
void PlayerSetOnDealtCard(TGMPlayer* player, TGMPlayerDealtCardFunction onDealtCard);
void PlayerSetOnPlayInGame(TGMPlayer* player, TGMPlayerPlayInGameFunction onPlayInGame);
void PlayerSetOnDestroy(TGMPlayer* player, TGMPlayerDestroyFunction onDestroy);
void PlayerSetOnDescription(TGMPlayer* player, TGMPlayerDescriptionFunction onDescription);
std::string PlayerDescription(TGMPlayer* player);

#define PlayerGetName(player) (player)->_name
#define PlayerGetHand(player) (player)->_hand
#define PlayerGetColor(player) (player)->_pc
#define PlayerIsTeam1(player) (player)->_isTeam1
#define PlayerGetStrategy(player) (player)->_strategy
#define PlayerGetContext(player) (player)->_context


// defined for UTs
void PlayerGameStarting(TGMPlayer* player, struct TGMGameStruct* game);
