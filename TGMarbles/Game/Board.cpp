//
//	Board.m
//	Marbles
//
//	Created by todd harris on 11/26/11.
//	Copyright 2012 toadgee.com. All rights reserved.
//

#include "precomp.h"

#include "Board.h"
#include "DebugAssert.h"
#include "MarblesMem.h"

#define kPlayersMarbleHomeSpotMarker -1
#define kPlayersMarbleFinalSpotMarker kTotalSpots

void ResetBoardInternal(TGMBoard *board, bool releaseMarbles);

TGMBoard* CreateBoard(void)
{
	MemIncreaseGlobalCount(g_boardsLiving);

	TGMBoard* board = static_cast<TGMBoard*>(malloc(sizeof(TGMBoard)));
	ResetBoardInternal(board, false);

#ifdef BOARD_MEMORY_LOGGING
	int64_t bcc = MemIncreaseGlobalCount(g_boardCreateCount);
	NSLog(@"<<BOARD %p : %d (+) %lld>>", board, 1, ccc);
#endif

	return board;
	
}

TGMBoard* CopyBoard(TGMBoard *board)
{
	TGMBoard* copy = CreateBoard();
	
	for (int spot = 0; spot < kTotalSpots; ++spot)
	{
		TGMMarble* marble = board->_board[spot];
		copy->_board[spot] = marble ? CopyMarble(marble) : nullptr;
	}

	for (int player = 0; player < kPlayers; ++player)
	{
		copy->_finalSpotCounts[player] = board->_finalSpotCounts[player];
		copy->_playerMarblesInPlay[player] = board->_playerMarblesInPlay[player];
		
		for (int marble = 0; marble < kMarblesPerPlayer; ++marble)
		{
			copy->_finalSpots[player][marble] = board->_finalSpots[player][marble];
			copy->_playerMarbles[player][marble] = board->_playerMarbles[player][marble];
		}
	}
	
	return copy;
}

void DestroyBoard(TGMBoard *board)
{
	if (!board)
		return;
	
	for (unsigned i = 0; i < kTotalSpots; i++)
	{
		if (board->_board[i] != nullptr)
		{
			ReleaseMarble(board->_board[i]);
		}
	}
	
#ifdef BOARD_MEMORY_LOGGING
	NSLog(@"<<BOARD %p : %d (-)>>", board, 0);
#endif
	
#ifdef DEBUG
	memset(board, static_cast<int>(0xDEADBEEF), sizeof(TGMBoard));
#endif
	MemDecreaseGlobalCount(g_boardsLiving);
	free(board);
}

void ResetBoard(TGMBoard *board)
{
	ResetBoardInternal(board, true);
}

void ResetBoardInternal(TGMBoard *board, bool releaseMarbles)
{
	dassert(board != nullptr);
	for (unsigned i = 0; i < kTotalSpots; i++)
	{
		if (releaseMarbles)
		{
			if (board->_board[i] != nullptr)
			{
				ReleaseMarble(board->_board[i]);
			}
		}
		
		board->_board[i] = nullptr;
	}
	
	for (unsigned i = 0; i < kPlayers; i++)
	{
		board->_finalSpotCounts[i] = 0;
		board->_playerMarblesInPlay[i] = 0;
		for (unsigned j = 0; j < kMarblesPerPlayer; j++)
		{
			board->_finalSpots[i][j] = MarbleColor::None;
			board->_playerMarbles[i][j] = kPlayersMarbleHomeSpotMarker;
		}
	}
}

void BoardMoveMarble(TGMBoard *board, int8_t oldSpot, int8_t newSpot)
{
	dassert(board != nullptr);
	dassert(IsValidSpot(oldSpot));
	dassert(IsValidSpot(newSpot));
	dassert(board->_board[oldSpot] != nullptr);
	dassert(board->_board[newSpot] == nullptr);
	
	TGMMarble* marble = board->_board[oldSpot];
	MarbleColor mc = marble->color;
	board->_board[oldSpot] = nullptr;
	board->_board[newSpot] = marble;
	
	PlayerColor pc = PlayerColorForMarbleColor(mc);
	int pos = PositionForPlayerColor(pc);
	int offset = MarbleColorToOffset(mc);
	dassert(board->_playerMarbles[pos][offset] == oldSpot);
	board->_playerMarbles[pos][offset] = newSpot;
}

int8_t BoardCountMarblesInPlayForPlayer(TGMBoard *board, PlayerColor pc)
{
	dassert(board != nullptr);
	dassert(IsPlayerColor(pc));
	
	int pos = PositionForPlayerColor(pc);
	
#ifdef DEBUG
	uint8_t inPlay = 0;
	for (int m = 0; m < kMarblesPerPlayer; m++)
	{
		int pm = board->_playerMarbles[PositionForPlayerColor(pc)][m];
		if (pm != kPlayersMarbleHomeSpotMarker && pm != kPlayersMarbleFinalSpotMarker)
		{
			inPlay++;
		}
	}
	dassert(board->_playerMarblesInPlay[pos] == inPlay);
#endif
	
	return board->_playerMarblesInPlay[pos];
}

TGMMarble *BoardMarbleAtSpot(TGMBoard *board, int spot)
{
	dassert(board != nullptr);
	dassert(IsValidSpot(spot));
	RetainMarble(board->_board[spot]);
	return board->_board[spot];
}

MarbleColor BoardMarbleColorAtSpot(TGMBoard *board, int spot)
{
	dassert(board != nullptr);
	dassert(IsValidSpot(spot));
	
	TGMMarble* marble = board->_board[spot];
	return (marble == nullptr) ? MarbleColor::None : marble->color;
}

MarbleColor BoardMarbleColorInFinalSpot(TGMBoard *board, int finalSpot, PlayerColor pc)
{
	dassert(board != nullptr);
	dassert(IsValidFinalSpot(finalSpot));
	dassert(IsPlayerColor(pc));
	
	return board->_finalSpots[PositionForPlayerColor(pc)][finalSpot];
}

int BoardFirstMarbleInFinalSpots(TGMBoard *board, int startingSpot, PlayerColor pc)
{
	dassert(board != nullptr);
	dassert(IsPlayerColor(pc));
	
	int pos = PositionForPlayerColor(pc);
	for (int fs = startingSpot; fs < kMarblesPerPlayer; fs++)
	{
		if (board->_finalSpots[pos][fs] != MarbleColor::None)
		{
			return fs;
		}
	}
	
	return kMarblesPerPlayer;
}

void BoardPlaceMarble(TGMBoard* board, TGMMarble* marble, int spot)
{
	dassert(board != nullptr);
	dassert(IsValidSpot(spot));
	dassert(marble != nullptr);
	dassert(board->_board[spot] == nullptr);
	
	board->_board[spot] = marble;
	RetainMarble(marble);
	
	MarbleColor mc = marble->color;
	PlayerColor pc = PlayerColorForMarbleColor(mc);
	int offset = MarbleColorToOffset(mc);
	int pos = PositionForPlayerColor(pc);
	dassert(board->_playerMarbles[pos][offset] == kPlayersMarbleHomeSpotMarker);
	board->_playerMarbles[pos][offset] = static_cast<int8_t>(spot);
	board->_playerMarblesInPlay[pos]++;
}

void BoardRemoveMarble(TGMBoard* board, int spot)
{
	dassert(board != nullptr);
	dassert(IsValidSpot(spot));
	dassert(board->_board[spot] != nullptr);
	
	MarbleColor mc = board->_board[spot]->color;
	PlayerColor pc = PlayerColorForMarbleColor(mc);
	int pos = PositionForPlayerColor(pc);
	int offset = MarbleColorToOffset(mc);
	
	dassert(board->_playerMarbles[pos][offset] == spot);
	board->_playerMarbles[pos][offset] = kPlayersMarbleHomeSpotMarker;
	board->_playerMarblesInPlay[pos]--;
	
	ReleaseMarble(board->_board[spot]);
	board->_board[spot] = nullptr;
}

uint8_t BoardCountScoreForPlayer(TGMBoard *board, PlayerColor pc)
{
	dassert(board != nullptr);
	dassert(IsPlayerColor(pc));
	
	int pos = PositionForPlayerColor(pc);
	uint8_t score = 0;
	for (int finalSpot = kMarblesPerPlayer - 1; finalSpot >= 0; --finalSpot)
	{
		if (board->_finalSpots[pos][finalSpot] != MarbleColor::None)
		{
			score++;
		}
		else
		{
			break;
		}
	}
	
	return score;
}

uint8_t BoardCountMarblesInFinalSpotForPlayer(TGMBoard *board, PlayerColor pc)
{
	dassert(board != nullptr);
	dassert(IsPlayerColor(pc));
	
	int pos = PositionForPlayerColor(pc);
	return board->_finalSpotCounts[pos];
}

void BoardPlaceMarbleAtFinalSpot(TGMBoard* board, MarbleColor mc, PlayerColor pc, int finalSpot)
{
	int pos = PositionForPlayerColor(pc);
	int offset = MarbleColorToOffset(mc);
	
	dassert(board != nullptr);
	dassert(IsValidFinalSpot(finalSpot));
	dassert(IsValidMarbleColor(mc));
	dassert(IsPlayerColor(pc));
	dassert(PlayerColorForMarbleColor(mc) == pc);
	dassert(board->_finalSpots[pos][finalSpot] == MarbleColor::None);
	dassert(board->_playerMarbles[pos][offset] != kPlayersMarbleFinalSpotMarker);
	
	board->_playerMarbles[pos][offset] = kPlayersMarbleFinalSpotMarker;
	board->_finalSpots[pos][finalSpot] = mc;
	board->_finalSpotCounts[pos]++;
}

void BoardRemoveMarbleAtFinalSpot(TGMBoard* board, PlayerColor pc, int finalSpot)
{
	dassert(board != nullptr);
	dassert(IsValidFinalSpot(finalSpot));
	dassert(IsPlayerColor(pc));
	
	int pos = PositionForPlayerColor(pc);
	
	MarbleColor mc = board->_finalSpots[pos][finalSpot];
	int offset = MarbleColorToOffset(mc);
	dassert(mc != MarbleColor::None);
	dassert(board->_playerMarbles[pos][offset] == kPlayersMarbleFinalSpotMarker);
	
	board->_playerMarbles[pos][offset] = kPlayersMarbleHomeSpotMarker;
	board->_finalSpots[pos][finalSpot] = MarbleColor::None;
	board->_finalSpotCounts[pos]--;
}

void BoardMoveMarbleAtFinalSpot(TGMBoard* board, PlayerColor pc, int oldFinalSpot, int newFinalSpot)
{
	int pos = PositionForPlayerColor(pc);
	
	dassert(board != nullptr);
	dassert(IsValidFinalSpot(oldFinalSpot));
	dassert(IsValidFinalSpot(newFinalSpot));
	
	MarbleColor mc = board->_finalSpots[pos][oldFinalSpot];
	dassert(mc != MarbleColor::None);
	dassert(board->_finalSpots[pos][newFinalSpot] == MarbleColor::None);
	dassert(board->_playerMarbles[pos][MarbleColorToOffset(mc)] == kPlayersMarbleFinalSpotMarker);
	
	board->_finalSpots[pos][oldFinalSpot] = MarbleColor::None;
	board->_finalSpots[pos][newFinalSpot] = mc;
}

int8_t BoardFindSpotWithPlayerColor(TGMBoard* board, PlayerColor pc, int initialSpot)
{
	dassert(board != nullptr);
	dassert(IsValidSpot(initialSpot));
	dassert(IsPlayerColor(pc));
	
	int posForPlayerColor = PositionForPlayerColor(pc);
	int8_t next = -1;
	for (int i = 0; i < kMarblesPerPlayer; i++)
	{
		int8_t pos = board->_playerMarbles[posForPlayerColor][i];
		if (pos != kPlayersMarbleFinalSpotMarker && pos >= initialSpot && (next == -1 || pos < next))
		{
			next = pos;
		}
	}
	
	return next;
}

int8_t BoardFindSpotWithMarbleColor(TGMBoard* board, MarbleColor mc)
{
	dassert(board != nullptr);
	dassert(IsMarbleColor(mc));
	
	int8_t spot = -1;
	for (int8_t i = 0; i < kTotalSpots; i++)
	{
		if (board->_board[i] != nullptr && board->_board[i]->color == mc)
		{
			spot = i;
			break;
		}
	}
	
	return spot;
}

bool AreBoardsEqual(TGMBoard *board, TGMBoard *other)
{
	dassert(board != nullptr);
	dassert(other != nullptr);
	for (unsigned i = 0; i < kTotalSpots; i++)
	{
		TGMMarble* m1 = board->_board[i];
		TGMMarble* m2 = other->_board[i];
		
		if (!AreMarblesEqual(m1, m2))
		{
			return false;
		}
	}
	
	for (unsigned p = 0; p < kPlayers; p++)
	{
		for (unsigned m = 0; m < kMarblesPerPlayer; m++)
		{
			MarbleColor mc1 = board->_finalSpots[p][m];
			MarbleColor mc2 = other->_finalSpots[p][m];
			if (mc1 != mc2)
			{
				return false;
			}
		}
	}
	
	return true;
}

int8_t SlowInternalGetUnusedMarbleColorCountForPlayer(TGMBoard *board, PlayerColor pc)
{
	dassert(board != nullptr);
	int8_t unused = kMarblesPerPlayer;
	for (int8_t i = 0; i < kMarblesPerPlayer; i++)
	{
		bool found = false;
		MarbleColor color = GetMarbleForPlayer(pc, i);
		for (int8_t spot = 0; spot < kTotalSpots; spot++)
		{
			TGMMarble* marble = board->_board[spot];
			if (marble != nullptr && marble->color == color)
			{
				found = true;
				break;
			}
		}
		
		if (!found)
		{
			// check the final spots
			for (int8_t j = 0; j < kMarblesPerPlayer; j++)
			{
				if (board->_finalSpots[PositionForPlayerColor(pc)][j] == color)
				{
					found = true;
					break;
				}
			}
		}
		
		if (found)
		{
			unused--;
		}
	}
	
	return unused;
}
