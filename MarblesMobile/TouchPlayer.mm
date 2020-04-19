//
//	TouchPlayer.m
//	Marbles
//
//	Created by toddha on 12/17/12.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import "precomp.h"
#import "DebugAssert.h"
#import "Descriptions.h"
#import "TouchPlayer.h"
#import "Game.h"
#import "MoveList.h"
#import "MoveGenerator.h"

#define kTurnStartedNotification @"MainPlayerTurnStarted"
#define TGMTouchPlayer struct TGMTouchPlayerStruct
struct TGMTouchPlayerStruct
{
	CFTypeRef _playLock;
	TGMMove* _move;
	BOOL _automaticallyDiscardWhenItsOurTurn;
	BOOL _isMyTurn;
};

void TouchPlayerPlayInGame(TGMPlayer* player);
std::string TouchPlayerDescription(TGMPlayer* player);
BOOL TouchPlayerDoAutoDiscardInGame(TGMPlayer* player, TGMGame* game);
void TouchPlayerSetIsMyTurn(TGMPlayer *player, BOOL isMyTurn);
TGMMove* TouchPlayerGetMove(TGMPlayer* player);
void TouchPlayerLock(TGMPlayer* player, int condition);
void TouchPlayerUnlock(TGMPlayer* player, int condition);
void TouchPlayerDestroy(TGMPlayer *player);
BOOL TouchPlayerReplaceMoveWithValidMoveInGame(TGMPlayer* player, TGMGame* game);

#define kConditionFinishedPlaying 1
#define kConditionWaitingForPlay 2

TGMPlayer* CreateTouchPlayer(const char* name, PlayerColor pc, TGMGame* game)
{
	TGMPlayer* player = CreatePlayer(name, Strategy::Human, pc);
	
	TGMTouchPlayer* touchPlayer = (TGMTouchPlayer*)malloc(sizeof(TGMTouchPlayer));
	touchPlayer->_playLock = (__bridge_retained CFTypeRef)[[NSConditionLock alloc] initWithCondition:kConditionWaitingForPlay];
	touchPlayer->_move = NULL;
	touchPlayer->_automaticallyDiscardWhenItsOurTurn = NO;
	touchPlayer->_isMyTurn = NO;
	
	PlayerSetContext(player, touchPlayer);
	PlayerSetOnPlayInGame(player, &TouchPlayerPlayInGame);
	PlayerSetOnDescription(player, &TouchPlayerDescription);
	PlayerSetOnDestroy(player, &TouchPlayerDestroy);
	
	return player;
}

void TouchPlayerDestroy(TGMPlayer *player)
{
	TGMTouchPlayer* touchPlayer = (TGMTouchPlayer *)PlayerGetContext(player);
	ReleaseMove(touchPlayer->_move);
	__unused NSConditionLock *lock = (__bridge_transfer NSConditionLock *)touchPlayer->_playLock;
	touchPlayer->_playLock = nullptr;
	free(touchPlayer);
}

void TouchPlayerSetMove(TGMPlayer* player, TGMMove* move)
{
	TGMTouchPlayer* touchPlayer = (TGMTouchPlayer *)PlayerGetContext(player);
	if (touchPlayer->_move != move)
	{
		if (touchPlayer->_move != NULL)
		{
			ReleaseMove(touchPlayer->_move);
		}
		
		if (move == NULL)
		{
			touchPlayer->_move = move;
		}
		else
		{
			touchPlayer->_move = RetainMove(move);
		}
	}
}

TGMMove* TouchPlayerGetMove(TGMPlayer* player)
{
	TGMTouchPlayer* touchPlayer = (TGMTouchPlayer *)PlayerGetContext(player);
	return touchPlayer->_move;
}

BOOL TouchPlayerGetAutomaticallyDiscardWhenItsOurTurn(TGMPlayer* player)
{
	TGMTouchPlayer* touchPlayer = (TGMTouchPlayer *)PlayerGetContext(player);
	return touchPlayer->_automaticallyDiscardWhenItsOurTurn;
}

void TouchPlayerSetAutomaticallyDiscardWhenItsOurTurn(TGMPlayer* player, BOOL automaticallyDiscardWhenItsOurTurn)
{
	TGMTouchPlayer* touchPlayer = (TGMTouchPlayer *)PlayerGetContext(player);
	touchPlayer->_automaticallyDiscardWhenItsOurTurn = automaticallyDiscardWhenItsOurTurn;
	
	if (automaticallyDiscardWhenItsOurTurn && TouchPlayerIsMyTurn(player))
	{
		if (TouchPlayerDoAutoDiscardInGame(player, PlayerGetGame(player)))
		{
			TouchPlayerFinishedPlaying(player);
		}
	}
}

BOOL TouchPlayerIsMyTurn(TGMPlayer *player)
{
	TGMTouchPlayer* touchPlayer = (TGMTouchPlayer *)PlayerGetContext(player);
	return touchPlayer->_isMyTurn;
}

void TouchPlayerSetIsMyTurn(TGMPlayer *player, BOOL isMyTurn)
{
	TGMTouchPlayer* touchPlayer = (TGMTouchPlayer *)PlayerGetContext(player);
	touchPlayer->_isMyTurn = isMyTurn;
}

void TouchPlayerLock(TGMPlayer* player, int condition)
{
	TGMTouchPlayer* touchPlayer = (TGMTouchPlayer *)PlayerGetContext(player);
	[(__bridge NSConditionLock *)touchPlayer->_playLock lockWhenCondition:condition];
}

void TouchPlayerUnlock(TGMPlayer* player, int condition)
{
	TGMTouchPlayer* touchPlayer = (TGMTouchPlayer *)PlayerGetContext(player);
	[(__bridge NSConditionLock *)touchPlayer->_playLock unlockWithCondition:condition];
}

void TouchPlayerPlayInGame(TGMPlayer* player)
{
	TGMGame* game = PlayerGetGame(player);
	
	TouchPlayerSetIsMyTurn(player, YES);
	
	__block BOOL letUserPlay = YES;
	// TODO : This causes hangs if the user touches the discard while we're playing...
	if (TouchPlayerGetAutomaticallyDiscardWhenItsOurTurn(player))
	{
		dispatch_sync(dispatch_get_main_queue(), ^
		{
			// only let the user play if the auto discard failed.
			letUserPlay = !TouchPlayerDoAutoDiscardInGame(player, game);
		});
	}
	
	if (letUserPlay)
	{
		[[NSNotificationCenter defaultCenter] postNotificationName:kTurnStartedNotification object:nil];
	}
	
	int condition = kConditionWaitingForPlay;
	do
	{
		TouchPlayerLock(player, kConditionFinishedPlaying);
		
		if (TouchPlayerGetMove(player) != nil)
		{
			// match up the moves with what we know the valid moves are
			// by just positions (note that this replaces self.move)
			if (TouchPlayerReplaceMoveWithValidMoveInGame(player, game))
			{
				condition = kConditionFinishedPlaying;
				GameDoMove(game, TouchPlayerGetMove(player));
			}
			
			TouchPlayerSetMove(player, NULL);
		}
		
		if (condition == kConditionWaitingForPlay)
		{
			TouchPlayerSetIsMyTurn(player, YES);
		}
		
		TouchPlayerUnlock(player, condition);
	}
	while (condition == kConditionWaitingForPlay);
}

void TouchPlayerFinishedPlaying(TGMPlayer* player)
{
	TouchPlayerLock(player, kConditionWaitingForPlay);
	TouchPlayerSetIsMyTurn(player, NO);
	TouchPlayerUnlock(player, kConditionFinishedPlaying);
}

BOOL TouchPlayerReplaceMoveWithValidMoveInGame(TGMPlayer* player, TGMGame* game)
{
	BOOL validated = NO;
	TGMMoveList* moveList = MovesForPlayerSimple(player, game, NULL);
	TGMMove *move = moveList->first;
	while (move != nullptr)
	{
		validated = AreMovesEquivalent(move, TouchPlayerGetMove(player));
		if (validated)
		{
			TouchPlayerSetMove(player, move);
			break;
		}
		
		move = move->nextMove;
	}
	
	ReleaseMoveList(moveList);
	return validated;
}

BOOL TouchPlayerDoAutoDiscardInGame(TGMPlayer* player, TGMGame* game)
{
	// sometimes this will fail when we get killed but have another get out (i.e. King) that we
	// otherwise would have discarded
	dassert([NSThread isMainThread]);
	
	TGMMoveList* moves = MovesForPlayerSimple(player, game, NULL);
	if (MoveListCount(moves) == 0)
	{
		ReleaseMoveList(moves);
		TouchPlayerSetAutomaticallyDiscardWhenItsOurTurn(player, NO);
		return NO;
	}
	
	BOOL automaticallyDiscard = YES;
	TGMMove* discardMove = NULL;
	int i = 0;
	TGMMove *move = moves->first;
	while (move != nullptr)
	{
		if (i == 0)
		{
			if (!move->isDiscard)
			{
				automaticallyDiscard = NO;
			}
			else
			{
				discardMove = move;
			}
		}
		
		++i;
		move = move->nextMove;
	}
	
	if (!automaticallyDiscard)
	{
		TouchPlayerSetAutomaticallyDiscardWhenItsOurTurn(player, NO);
		ReleaseMoveList(moves);
		return NO;
	}
	
	TouchPlayerSetMove(player, discardMove);
	ReleaseMoveList(moves);
	return YES;
}

std::string TouchPlayerDescription(TGMPlayer* player)
{
    NSString *description = [NSString stringWithFormat:@"[%d] TouchPlayer (%s)",
		PlayerIsTeam1(player) ? 1 : 2,
		PlayerColorToString(PlayerGetColor(player))];
	std::string descriptionStr { [description UTF8String] };
	return descriptionStr;
}
