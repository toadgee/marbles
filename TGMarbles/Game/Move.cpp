#include "precomp.h"
#include "Move.h"
#include "DebugAssert.h"
#include "Descriptions.h"
#include "MoveList.h"
#include "Board.h"
#include "Card.h"
#include "Player.h"
#include "Board.h"
#include "Logging.h"
#include "Game.h"
#include "Marble.h"
#include "MarblesMem.h"

int MoveCalculateIntermediateSpotsInternal(TGMMove* move, int moves, int fromSpot, int spotMarker, bool wrapSpot);
bool IsMoveValid_DoesPassOrKillOurMarbles(TGMMove* move, PlayerColor pc, TGMBoard* board);
TGMMove* AllocateMove(void);
void DeallocateMove(TGMMove* move);

static std::mutex s_lock;
static TGMPoolAllocator* s_MoveAllocationPoolUnusedFirst = NULL;
static TGMPoolAllocator* s_MoveAllocationPoolUsedFirst = NULL;

TGMMove* AllocateMove(void)
{
	const std::lock_guard<std::mutex> lock(s_lock);

	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMMove), &s_MoveAllocationPoolUnusedFirst, &s_MoveAllocationPoolUsedFirst);
	TGMMove* move = (TGMMove *)allocator->_object;
	move->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	MemIncreaseGlobalCount(g_movesLiving);
	
	return move;
}

void DeallocateMove(TGMMove* move)
{
	dassert(move);
	if (!move) return;
	
	TGMPoolAllocator* allocator = (TGMPoolAllocator*)move->_holder; // need to do this before memset...
#ifdef DEBUG
	memset(move, (int)0xDEADBEEF, sizeof(TGMMove));
	move->_retainCount = 0;
	move->_holder = allocator;
#endif

	const std::lock_guard<std::mutex> lock(s_lock);
	DeallocatePoolObject(allocator, &s_MoveAllocationPoolUnusedFirst, &s_MoveAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_movesLiving);
}

TGMMove* MakeMoveUnfiltered(
	TGMCard* card,
	TGMMarble* marble,
	PlayerColor playerColor,
	CardNumber cardNum, 
	int moves, 
	int oldSpot, 
	int newSpot, 
	int normalizedOldSpot, 
	int jumps,
	int unusedMarbles)
{
	dassert(IsCardNumber(cardNum));
	
	// note that card.card may not be cardNum if card.card was a wild card -- thus we fall back on to cardNum
	// Kings can only be get-outs unless we don't have any more marbles
	if (cardNum == CardNumber::King && unusedMarbles != 0)
	{
		return NULL;
	}
	
	bool wentBehindHome = false;
	bool forwardProgress = (moves > 0);
	int movedDistance = (newSpot - oldSpot + kTotalSpots) % kTotalSpots;
	if (forwardProgress)
	{
		// 0 is a special spot - so if it's already been around, we shouldn't do a non-get in move
		if (normalizedOldSpot == 0 && marble->wentBehindHome)
		{
			return NULL;
		}
		
		// cards can't move past kTotalSpots : either -kTotalSpots or +kTotalSpots (not including final spots) (part 1)
		if ((marble->distanceFromHome + movedDistance) > (kTotalSpots + kMarblesPerPlayer))
		{
			return NULL;
		}
		
		// normalize the spots down to the player's perspective of 0 to kMaxSpots
		wentBehindHome = (normalizedOldSpot > NormalizeSpot(playerColor, newSpot));
		
		// we can only move into home via forward progress
		if (marble->distanceFromHome + movedDistance > kTotalSpots)
		{
			PlayerColor pc = PlayerColorForMarbleColor(marble->color);
			newSpot = FinalSpotToSpot(NormalizeSpot(pc, newSpot - 1)); // subtract 1 since they're 0 based
		}
	}
	else 
	{
		// cards can't move past kTotalSpots : either -kTotalSpots or +kTotalSpots (not including final spots) (part 2)
		if (marble->distanceFromHome + movedDistance < (0 - kTotalSpots))
		{
			return NULL;
		}
		
		// normalize the spots down to the player's perspective of 0 to kMaxSpots
		wentBehindHome = (normalizedOldSpot < NormalizeSpot(playerColor, newSpot));
	}
	
	return MakeMove(card, marble, false, playerColor, oldSpot, newSpot, moves, jumps, wentBehindHome);
}

TGMMove* MakeMove(
	TGMCard* card,
	TGMMarble* marble,
	bool isDiscard,
	PlayerColor playerColor,
	int oldSpot,
	int newSpot,
	int moves,
	int jumps,
	bool wentBehindHome)
{
#ifdef DEBUG
	dassert(isDiscard || moves > 0 || moves == -4 || (moves == 0 && oldSpot == kGetOutSpot));
	dassert(newSpot >= 0);
	dassert(IsPlayerColor(playerColor));
	dassert(!(oldSpot == 0 && newSpot == 0 && !IsFinalSpot(newSpot)) || isDiscard);
	dassert(jumps >= 0 && jumps <= 5);
#endif
	
	TGMMove* move = AllocateMove();
	move->_retainCount = 1;
	move->marble = marble;
	if (marble) RetainMarble(marble);
	move->card = card;
	RetainCard(card);
	move->playerColor = playerColor;
	move->oldSpot = oldSpot;
	move->newSpot = newSpot;
	move->isDiscard = isDiscard;
	move->moves = moves;
	move->wentBehindHome = wentBehindHome;
	move->jumps = jumps;
	move->weight = 0;
	move->weightCalculated = false;
	move->nextMove = NULL;
	move->previousMove = NULL;
	move->spotsCalculated = false;
	dassert(jumps != -1);
	
#ifdef MOVE_MEMORY_LOGGING
	MemIncreaseGlobalCount(g_moveCreateCount);
	NSLog(@"<<MOVE %p : %d (+) %lld>>", move, move->_retainCount, g_moveCreateCount);
#endif
	
	// do this last so at least we can print it out :)
	dassert(isDiscard || oldSpot == kGetOutSpot || IsFinalSpot(oldSpot) || marble != NULL);
	
	return move;
}

int MoveCalculateIntermediateSpotsInternal(TGMMove* move, int moves, int fromSpot, int spotMarker, bool wrapSpot)
{
	int adjMoves = moves;
	int adjSpot = 1;
	if (moves < 0)
	{
		// make adjMoves positive and adjust how we count the spot (either forward or backwards)
		adjMoves *= -1;
		adjSpot = -1;
	}
		
	for (int s = 0; s <= adjMoves; s++)
	{
		// for final spots, we don't want to wrap them because they're - by definition, final spots
		int newSpot = fromSpot + (adjSpot * s);
		if (wrapSpot)
			newSpot = WrapSpot(newSpot);
		
		move->spots[s + spotMarker] = (uint8_t)newSpot;
	}
	
	return (spotMarker + adjMoves);
}

void MoveCalculateIntermediateSpots(TGMMove* move)
{
	if (move->spotsCalculated || move->isDiscard)
		return;
	
	move->spotsCalculated = true;
	if (move->oldSpot == kGetOutSpot)
	{
		move->spots[0] = (uint8_t)move->newSpot;
	}
	else
	{
		int spotMarker = 0;
		if (!IsFinalSpot(move->oldSpot))
		{
			int jumps = move->jumps;
			int fromSpot = move->oldSpot;
			int moves = move->moves;
			int finalMoves = 0;
		
			if (IsFinalSpot(move->newSpot))
			{
				finalMoves = SpotToFinalSpot(move->newSpot);
				moves -= finalMoves;
			}
		
			if (jumps > 0)
			{
				int spotDiff = kPlayerSpots;
				int moveInc = -1;
				if (move->moves < 0)
				{
					spotDiff *= -1;
					moveInc = 1;
				}
			
				while (jumps > 0)
				{
					move->spots[spotMarker] = (uint8_t)fromSpot;
					spotMarker++;
				
					fromSpot = WrapSpot(fromSpot + spotDiff);
					dassert(IsPointSpot(fromSpot));
					jumps--;
					moves += moveInc;
				}
			}
		
			spotMarker = MoveCalculateIntermediateSpotsInternal(move, moves, fromSpot, spotMarker, true);
			if (IsFinalSpot(move->newSpot))
			{
				// do a sub calculation for final spots
				spotMarker = MoveCalculateIntermediateSpotsInternal(move, finalMoves, FinalSpotToSpot(0), spotMarker, false);
			}
		}
		else if (IsFinalSpot(move->oldSpot) && IsFinalSpot(move->newSpot))
		{
			spotMarker = MoveCalculateIntermediateSpotsInternal(move, move->moves, move->oldSpot, 0, false);
		}
		else
		{
			dassert(false);
			move->spotsCalculated = false;
		}
		
		// signify the end
		if (spotMarker + 1 < kMaxMoveSpots)
		{
			move->spots[spotMarker + 1] = kMoveSpotUnused;
		}
	}
	
#ifdef DEBUG
	if (move->spotsCalculated)
	{
		int c = 0;
		while (c < kMaxMoveSpots)
		{
			if (move->spots[c] == kMoveSpotUnused)
			{
				break;
			}
			
			c++;
		}
		
		int moves = move->moves;
		if (move->moves < 0) moves *= -1;
		if (c != moves + 1)
		{
			fprintf(stderr, "%s\n", MoveDescription(move).c_str());
			dassert(false);
		}
	}
#endif
}

TGMMove* CopyMove(TGMMove * move)
{
	TGMMove* copy = AllocateMove();
	void* holder = copy->_holder;
	memcpy(copy, move, sizeof(TGMMove));
	copy->_holder = holder;
	copy->_retainCount = 1;
	copy->nextMove = NULL;
	copy->previousMove = NULL;
	if (copy->marble)
	{
		RetainMarble(copy->marble);
	}
	RetainCard(copy->card);
	
#ifdef MOVE_MEMORY_LOGGING
	MemIncreaseGlobalCount(g_moveCreateCount);
	NSLog(@"<<MOVE %p : %d (+) %lld>>", copy, copy->_retainCount, g_moveCreateCount);
#endif
	
	return copy;
}

TGMMove* RetainMove(TGMMove *move)
{
	MemIncreaseRetainCount(move->_retainCount);
	RetainCard(move->card);
	if (move->marble != nullptr)
	{
		RetainMarble(move->marble);
	}
		
#ifdef MOVE_MEMORY_LOGGING
	NSLog(@"<<MOVE %p : %d (+)>>", move, move->_retainCount);
#endif
	
	return move;
}

void ReleaseMove(TGMMove* move)
{
	MemDecreaseRetainCount(move->_retainCount);
#ifdef MOVE_MEMORY_LOGGING
	NSLog(@"<<MOVE %p : %d (-)>>", move, move->_retainCount);
#endif

	dassert(move->_retainCount >= 0);
	ReleaseCard(move->card);
	if (move->marble != nullptr)
	{
		ReleaseMarble(move->marble);
	}
	
	if (move->_retainCount == 0)
	{
		DeallocateMove(move);
	}
}

bool IsMoveValid_DoesPassOrKillOurMarbles(TGMMove* move, PlayerColor pc, TGMBoard* board)
{
	if (move->isDiscard || move->oldSpot == kGetOutSpot)
	{
		return true;
	}
	
	// we need to calculate the internal spots for this move first so we know where it lands
	MoveCalculateIntermediateSpots(move);
	
	// we can effectively ignore spot[0] because this is the from spot - and we know we're in that
	// spot. we want to pay attention to all the rest because we might skip a marble (or kill it)
	int s = 1;
	while (s < kMaxMoveSpots)
	{
		int spot = move->spots[s];
		if (spot == kMoveSpotUnused)
		{
			break;
		}
		
		MarbleColor mc;
		if (IsFinalSpot(spot))
		{
			mc = BoardMarbleColorInFinalSpot(board, SpotToFinalSpot(spot), pc);
		}
		else
		{
			mc = BoardMarbleColorAtSpot(board, spot);
		}
		
		if (mc != MarbleColor::None && IsPlayersMarble(mc, pc))
		{
			return false;
		}
		
		s++;
	}
	
	return true;
}

bool IsMoveValidOnBoard(TGMMove* move, TGMBoard* board, PlayerColor pc)
{
	dassert(IsPlayerColor(move->playerColor));
	if (!IsMoveValid_DoesPassOrKillOurMarbles(move, pc, board)) return false;
	return true;
}

bool AreMovesEqual(TGMMove* move1, TGMMove* move2)
{
	if (move1 == move2)
		return true;
	if (!AreCardsEqual(move1->card, move2->card))
		return false;
	if (move1->oldSpot != move2->oldSpot)
		return false;
	if (move1->newSpot != move2->newSpot)
		return false;
	if (move1->isDiscard != move2->isDiscard)
		return false;
	if (move1->moves != move2->moves)
		return false;
	if (move1->jumps != move2->jumps)
		return false;
	if (move1->wentBehindHome != move2->wentBehindHome)
		return false;
	if (move1->playerColor != move2->playerColor)
		return false;
	
	return true;
}

bool AreMovesEquivalent(TGMMove* move1, TGMMove* move2)
{
	if (move1 == move2)
		return true;
	if (!AreCardsEquivalent(move1->card, move2->card))
		return false;
	if (move1->oldSpot != move2->oldSpot)
		return false;
	if (move1->newSpot != move2->newSpot)
		return false;
	if (move1->isDiscard != move2->isDiscard)
		return false;
	
	return true;
}

