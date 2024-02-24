#include "precomp.h"
#include "Card.h"
#include "DebugAssert.h"
#include "Move.h"
#include "Game.h"
#include "ComputerPlayer.h"
#include "MoveGenerator.h"
#include "MoveRanker.h"
#include "Board.h"

// by creating a move ranker struct, we don't have to recalculate some of the same data over and over
#define TGMMoveRankerData struct TGMMoveRankerDataStruct
struct TGMMoveRankerDataStruct
{
	TGMMove*move;
	
	TGMGame* game;
	
	TGMBoard *board;
	
	Strategy strategy;
	bool oldSpotIsFinalSpot;
	bool newSpotIsFinalSpot;
	PlayerColor playerColor;
	bool oldSpotIsPointSpot;
	bool newSpotIsPointSpot;
	
	int oldSpotNormalized;
	int newSpotNormalized;
};

void DoWeightOfMoveBasedOnLikelihoodOfBeingKilled(TGMMoveRankerData *data, TGMMoveList** allPossibleOpponentMoves);
void DoWeightOfMoveBasedOnMovement(TGMMoveRankerData *data);
void DoWeightOfMoveBasedOnStartingSpot(TGMMoveRankerData *data);
void DoWeightOfMoveIntoHomeBase(TGMMoveRankerData *data);
void DoWeightOfMoveIntoPlayerStartingSpot(TGMMoveRankerData *data);
void DoWeightOfMoveOutOfPlayerStartingSpot(TGMMoveRankerData *data);
void DoWeightOfMoveKill(TGMMoveRankerData *data);
void DoWeightOfGetOutMove(TGMMoveRankerData *data);
void DoWeightOfMoveFromPoint(TGMMoveRankerData *data);
void DoWeightOfMoveToPoint(TGMMoveRankerData *data);
void DoWeightOfDiscardMove(TGMMoveRankerData *data);
void DoWeightOfJokerMove(TGMMoveRankerData *data);
void DoWeightOfMoveFromHomeBackFour(TGMMoveRankerData *data);

void DoWeightOfMoveBasedOnLikelihoodOfBeingKilled(TGMMoveRankerData *data, TGMMoveList** allPossibleOpponentMoves)
{
	const int kSafeMoveWeight = 50;
	const int kUnsafeMoveWeight = 5; // per possible kill, subtraction!
	
	if (!IsDefensiveStrategy(data->strategy))
	{
		return;
	}
	
	if (allPossibleOpponentMoves == nullptr) [[unlikely]]
	{
		// huh, we weren't asked to calculate this...
		return;
	}
	
	int killedCount = 0;
	
	// final spot moves can't be killer moves
	if (!IsFinalSpot(data->move->newSpot))
	{
		// for every move by the opponents that could kill us, we decrease the weight by some amount
		// this is fairly expensive since we need to generate all moves for all other marbles on the board...
		bool opponentsAreTeam1 = !IsPlayerColorTeam1(data->playerColor);
		
		// only calculate opponent moves once across all moves (performance optimization)
		if (*allPossibleOpponentMoves == nullptr)
		{
			*allPossibleOpponentMoves = AllPossibleMovesForAllTeamPlayers(data->game, opponentsAreTeam1);
		}
		
		TGMMoveList *allPossibleMoves = *allPossibleOpponentMoves;
		
		{
			TGMMove* possibleKillMove = allPossibleMoves->first;
			while (possibleKillMove != nullptr)
			{
				// if there's a move that can kill us, we subtract some amount
				// if there's a move that can't, we add some amount
				// ignore final spot moves
				if (!IsFinalSpot(possibleKillMove->newSpot))
				{
					bool isKillerMove = (possibleKillMove->newSpot == data->move->newSpot);
					if (isKillerMove)
					{
						killedCount++;
					}
				}
				
				possibleKillMove = possibleKillMove->nextMove;
			}
		}
	}
	
	if (killedCount == 0)
	{
		// completely safe move
		data->move->weight += kSafeMoveWeight;
		return;
	}
	else
	{
		data->move->weight -= (kUnsafeMoveWeight * killedCount);
	}
}

void DoWeightOfMoveBasedOnMovement(TGMMoveRankerData *data)
{
	// this allows us to not have to worry about forwards/backwards progress as much
	int normalizedSpot1;
	int normalizedSpot2;
	bool forwardProgress = (data->move->moves >= 0);
	if (forwardProgress)
	{
		normalizedSpot1 = data->newSpotNormalized;
		normalizedSpot2 = data->oldSpotNormalized;
	}
	else
	{
		normalizedSpot1 = data->oldSpotNormalized;
		normalizedSpot2 = data->newSpotNormalized;
	}
	
	if (data->move->oldSpot != kGetOutSpot)
	{
		data->move->weight += WrapSpot(normalizedSpot1 - normalizedSpot2);
	}
	
	if (!data->oldSpotIsFinalSpot && !data->newSpotIsFinalSpot && data->oldSpotIsPointSpot)
	{
		data->move->weight += (kPlayerSpots - ((normalizedSpot1 - normalizedSpot2) % kPlayerSpots));
	}
}

void DoWeightOfMoveBasedOnStartingSpot(TGMMoveRankerData *data)
{
	// take into account distance from home - the further the distance, the more we've invested into that marble, the more
	// we should move it first
	if (!data->oldSpotIsFinalSpot && !data->newSpotIsFinalSpot)
	{
		data->move->weight += (data->oldSpotNormalized / 10);
	}
}

void DoWeightOfMoveIntoHomeBase(TGMMoveRankerData *data)
{
	// determine if home move
	// TODO : Improvement : good fits could be those which we have the cards to fit them in
	// determine if a move into home base
	if (data->newSpotIsFinalSpot)
	{
		// calculate how many marble spots are between this move's spot and the maximum spot
		// (accounting for other marbles)
		int newSpotFinalSpot = SpotToFinalSpot(data->move->newSpot);
		
		int firstMarbleSpot = BoardFirstMarbleInFinalSpots(data->board, newSpotFinalSpot, data->move->playerColor);
		int distanceFromNextMarbleInHome = firstMarbleSpot - newSpotFinalSpot - 1;
		
		// if this slots in right next to the next marble in our home, then we just need to double check
		// that there aren't any other marble holes in front of the new spot
		bool isFull = false;
		if (distanceFromNextMarbleInHome == 0)
		{
			isFull = true;
			for (int finalSpotCounter = newSpotFinalSpot + 1; finalSpotCounter < kMarblesPerPlayer; finalSpotCounter++)
			{
				MarbleColor mc = BoardMarbleColorInFinalSpot(data->board, finalSpotCounter, data->move->playerColor);
				if (mc == MarbleColor::None)
				{
					isFull = false;
					break;
				}
			}
		}
		
		if (distanceFromNextMarbleInHome == 0 && isFull)
		{
			// it's a good fit if all the rest of our home is filled
			data->move->weight += 500;
		}
		else if (distanceFromNextMarbleInHome == 0 || distanceFromNextMarbleInHome == 1 || distanceFromNextMarbleInHome == 4)
		{
			// it's a really bad fit if we need an ace or 4 - or there are other holes past where we would land
			if (IsPassiveStrategy(data->strategy))
			{
				data->move->weight += 50;
			}
			else if (IsAggressiveStrategy(data->strategy))
			{
				data->move->weight += 15;
			}
			else dassert(0);
		}
		else
		{
			// bad fit
			if (IsPassiveStrategy(data->strategy))
			{
				data->move->weight += 200;
			}
			else if (IsAggressiveStrategy(data->strategy))
			{
				data->move->weight += 80;
			}
			else dassert(0);
		}
	}
}

void DoWeightOfMoveIntoPlayerStartingSpot(TGMMoveRankerData *data)
{
	// determine if it's a move into a starting spot
	if (!data->newSpotIsFinalSpot && (IsPlayerStartingSpot(data->move->newSpot)))
	{
		if (IsPassiveStrategy(data->strategy))
		{
			data->move->weight += -50;
		}
		else if (IsAggressiveStrategy(data->strategy))
		{
			data->move->weight += 0; // no-op
		}
		else dassert(0);
	}
}

void DoWeightOfMoveOutOfPlayerStartingSpot(TGMMoveRankerData *data)
{
	// determine if it's a play out of a starting spot
	if (!data->oldSpotIsFinalSpot && (IsPlayerStartingSpot(data->move->oldSpot)) && data->move->marble != nullptr)
	{
		// we do a weird trick that says we know if it's in our home position by looking to see if the distance
		// is the same as the starting position's distance.
		bool isMarblesHomeSpot = (data->move->marble->distanceFromHome == kPlayerStartingPosition);
		if (!isMarblesHomeSpot)
		{
			// best not to hang around somebody else's starting spot for too long...
			data->move->weight += 20;
		}
	}
}

void DoWeightOfMoveKill(TGMMoveRankerData *data)
{
	// determine if kill move
	if (!data->oldSpotIsFinalSpot && !data->newSpotIsFinalSpot)
	{
		MarbleColor existingColor = BoardMarbleColorAtSpot(data->board, data->move->newSpot);
		if (existingColor != MarbleColor::None)
		{
			PlayerColor marblePlayerColor = PlayerColorForMarbleColor(existingColor);
			PlayerColor playerColor = data->playerColor;
			
			if (marblePlayerColor == playerColor || ArePlayersTeammates(marblePlayerColor, playerColor))
			{
				// kill teammate (or self)
				data->move->weight += -500;
			}
			else
			{
				// kill enemy
				if (IsPassiveStrategy(data->strategy))
				{
					data->move->weight += -75;
				}
				else if (IsAggressiveStrategy(data->strategy))
				{
					data->move->weight += 125;
				}
				else dassert(0);
			}
		}
	}
}

void DoWeightOfGetOutMove(TGMMoveRankerData *data)
{
	// determine if get out move (do this after kill moves)
	if (data->move->oldSpot == kGetOutSpot)
	{
		// we want to take into account how many other marbles for this player in play
		int8_t otherMarblesInPlay = BoardCountMarblesInPlayForPlayer(data->board, data->move->playerColor);
		
		if (IsPassiveStrategy(data->strategy))
		{
			data->move->weight += (0 + (otherMarblesInPlay * -5));
		}
		else if (IsAggressiveStrategy(data->strategy))
		{
			data->move->weight += (20 + (otherMarblesInPlay * -5));
		}
		else
		{
			dassert(0);
		}
		
		// basic order goes : use kings first, then aces, then jokers
		if (CardNumber(data->move->card) == CardNumber::King)
		{
			data->move->weight += 1;
		}
		else if (CardNumber(data->move->card) == CardNumber::Joker)
		{
			data->move->weight -=1;
		}
		else
		{
			dassert(CardNumber(data->move->card) == CardNumber::Ace);
		}
	}
}

void DoWeightOfMoveFromPoint(TGMMoveRankerData *data)
{
	// determine if it's a jump move (do this before other point moves)
	if (!data->newSpotIsFinalSpot && data->oldSpotIsPointSpot && data->move->jumps > 0)
	{
		data->move->weight += 5;
	}
}

void DoWeightOfMoveToPoint(TGMMoveRankerData *data)
{
	if (data->move->marble != nullptr)
	{
		PlayerColor playerColorForMarble = PlayerColorForMarbleColor(data->move->marble->color);
		
		// determine if it's a point move (move to the point, do this before jump moves -- although not sure it matters)
		// also the point before home doesn't really count as more
		if (!data->newSpotIsFinalSpot && data->newSpotIsPointSpot && !IsLastPlayerPointSpot(data->move->newSpot, playerColorForMarble))
		{
			if (data->oldSpotIsPointSpot)
			{
				// from point spot to point spot. not bad
				data->move->weight += 10;
			}
			else
			{
				// from point spot to non point spot. better(?)
				data->move->weight += 50;
			}
		}
	}
}

void DoWeightOfDiscardMove(TGMMoveRankerData *data)
{
	// TODO : this is pretty dumb today - but the general rule is if we're discarding, 
	if (data->move->isDiscard)
	{
		// we are only saved if another player on our team has a get out. if they do, the best cards
		// to have are 5s (and maybe 4s), so discard those last
		int baseWeight = -1500;
		if (CardNumber(data->move->card) == CardNumber::Card5)
		{
			baseWeight -= 100;
		}
		else if (CardNumber(data->move->card) == CardNumber::Card4)
		{
			baseWeight -= 50;
		}
		
		data->move->weight += baseWeight;
	}
}

void DoWeightOfJokerMove(TGMMoveRankerData *data)
{
	// joker moves should be considered less valuable since other cards that aren't jokers are better - so that 
	// two moves with a joker & non-joker, the non-joker comes out ahead
	if (CardNumber(data->move->card) == CardNumber::Joker)
	{
		data->move->weight += -1;
	}
}

void DoWeightOfMoveFromHomeBackFour(TGMMoveRankerData *data)
{
	if (CardNumber(data->move->card) == CardNumber::Card4
	 && data->oldSpotNormalized == 4 
	 && data->newSpotNormalized == 0)
	{
		if (IsPassiveStrategy(data->strategy))
		{
			data->move->weight += 50;
		}
		else if (IsAggressiveStrategy(data->strategy))
		{
			data->move->weight += 20;
		}
		else
		{
			dassert(0);
		}
	}
}

int CalculateWeightOfMoveInGame(TGMMove* move, TGMGame* game, PlayerColor pc, Strategy strategy, TGMMoveList** allPossibleOpponentMoves)
{
	// calculate weight of the move for comparison reasons
	// a higher weight is a better move
	
	// TODO : Do better in calculating move weight based on cards in game
	// for example, calculating moves in the final spot if we have an ace
	// 4 away from the final spot is bad unless we have a 3 and ace (in which case it's just not great)
	// 1 away from the final spot is difficult and is generally bad unless we have an ace or joker
	
	// only calculate this once per move.
	if (!move->weightCalculated)
	{
		move->weightCalculated = true;
		move->weight = 0;
		
		TGMMoveRankerData data;
		data.move = move;
		data.strategy = strategy;
		data.game = game;
		data.board = GameGetBoard(game);
		data.playerColor = pc;
		data.oldSpotIsFinalSpot = IsFinalSpot(move->oldSpot);
		data.newSpotIsFinalSpot = IsFinalSpot(move->newSpot);
		data.oldSpotIsPointSpot = IsPointSpot(move->oldSpot);
		data.newSpotIsPointSpot = IsPointSpot(move->newSpot);
		data.oldSpotNormalized = NormalizeSpot(move->playerColor, move->oldSpot);
		data.newSpotNormalized = NormalizeSpot(move->playerColor, move->newSpot);
		
		DoWeightOfMoveBasedOnLikelihoodOfBeingKilled(&data, allPossibleOpponentMoves);
		DoWeightOfMoveBasedOnMovement(&data);
		DoWeightOfMoveBasedOnStartingSpot(&data);
		DoWeightOfMoveIntoPlayerStartingSpot(&data);
		DoWeightOfMoveOutOfPlayerStartingSpot(&data);
		DoWeightOfMoveKill(&data);
		DoWeightOfMoveIntoHomeBase(&data);
		DoWeightOfGetOutMove(&data);
		DoWeightOfMoveFromPoint(&data);
		DoWeightOfMoveToPoint(&data);
		DoWeightOfJokerMove(&data);
		DoWeightOfDiscardMove(&data);
		DoWeightOfMoveFromHomeBackFour(&data);
	}
	
	return move->weight;
}
