//
//	ComputerPlayer.m
//	Marbles
//
//	Created by todd harris on 11/27/11.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#include "precomp.h"
#include "Board.h"
#include "Card.h"
#include "ComputerPlayer.h"
#include "DebugAssert.h"
#include "Descriptions.h"
#include "Game.h"
#include "Logging.h"
#include "MarblesMem.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "MoveList.h"
#include "MoveRanker.h"
#include "Player.h"

#define kStrategyAdaptive1Threshhold 5
#define kStrategyAdaptive2Threshhold 5


#define TGMComputerPlayer struct TGMComputerPlayerStruct
struct TGMComputerPlayerStruct
{
	int _passiveAggressive;
};


void ComputerPlayerDestroy(TGMPlayer *player);

void ComputerPlayerPlayInGame(TGMPlayer* player);

TGMPlayer* CreateComputerPlayer(const char* name, Strategy strategy, PlayerColor pc)
{
	TGMPlayer* player = CreatePlayer(name, strategy, pc);
	
	PlayerSetOnPlayInGame(player, &ComputerPlayerPlayInGame);
	PlayerSetOnDescription(player, &ComputerPlayerDescription);

	TGMComputerPlayer* computerPlayer = static_cast<TGMComputerPlayer*>(malloc(sizeof(TGMComputerPlayer)));
	computerPlayer->_passiveAggressive = 0;
	PlayerSetContext(player, computerPlayer);
	PlayerSetOnDestroy(player, &ComputerPlayerDestroy);
	
	return player;
}

void ComputerPlayerDestroy(TGMPlayer *player)
{
	TGMComputerPlayer* computerPlayer = static_cast<TGMComputerPlayer *>(PlayerGetContext(player));
	free(computerPlayer);
}

void ComputerPlayerPlayInGame(TGMPlayer* player)
{
	Strategy s = player->_strategy;
	if (s == Strategy::PassiveAggressive)
	{
		// for passive/agressive switch between the two
		TGMComputerPlayer* computerPlayer = static_cast<TGMComputerPlayer*>(PlayerGetContext(player));
		computerPlayer->_passiveAggressive++;
		
		s = (computerPlayer->_passiveAggressive % 2 == 0) ? Strategy::Passive : Strategy::Aggressive;
	}
	
	ComputerPlayerPlayInGameWithStrategy(player, s);
}

void ComputerPlayerPlayInGameWithStrategy(TGMPlayer* player, Strategy s)
{
	// perf note : it seems to be more intensive to just find the unique cards than
	// it does to just always use all cards and deal with a slightly larger move set
	TGMMoveList* moves = MovesForPlayerSimple(player, player->_game, nullptr);
	
	if (s == Strategy::PassiveAggressive) [[unlikely]]
	{
		assert(false);
	}
	else if (s == Strategy::Adaptive1 || s == Strategy::Adaptive2)
	{
		// for adaptive, count the number of marbles that aren't in play and then decide
		// based on that number how to play
		int unusedMarbleCount = 0;
		PlayerColor color = OtherTeamColor(player->_pc);
		PlayerColor first = color;
		do
		{
			unusedMarbleCount += PlayerUnusedMarbleCount(GamePlayerForColor(player->_game, color));
			color = NextTeammateColor(color);
		}
		while (first != color);
		
		if (s == Strategy::Adaptive1)
		{
			s = (unusedMarbleCount <= kStrategyAdaptive1Threshhold) ? Strategy::Aggressive : Strategy::Passive;
		}
		else if (s == Strategy::Adaptive2)
		{
			s = (unusedMarbleCount <= kStrategyAdaptive2Threshhold) ? Strategy::Passive : Strategy::Aggressive;
		}
	}
	
	TGMMove* move;
	if (moves->first->isDiscard)
	{
		// TODO
		/*
#ifdef DEBUG_LOGGING_ON
		TGMLogGameMessage(@"%@ -- has to discard. no possible moves", self);
#endif
		*/
		
		move = BestDiscardMoveInGameFromMoves(moves);
		dassert(move != nullptr);
	}
	else
	{
		move = BestMoveFromMoves(moves, player->_game, player->_pc, s);
		dassert(move != nullptr);
	}
	
	ReleaseMoveList(moves);
	
	// finally do the move
	GameDoMove(player->_game, move);
	ReleaseMove(move);
}
