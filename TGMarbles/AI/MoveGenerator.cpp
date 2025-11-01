#include "precomp.h"
#include "DebugAssert.h"
#include "MoveGenerator.h"
#include "Player.h"
#include "Game.h"
#include "Card.h"
#include "MoveRanker.h"
#include "MarblesMem.h"

void StartingMovesForPlayer(
	TGMMoveList *moveList,
	PlayerColor pc,
	int unusedMarbles, 
	TGMCardList* hand, 
	TGMBoard* board);
	
void GenerateDiscardMovesForPlayer(
	TGMMoveList* moveList,
	PlayerColor pc, 
	TGMCardList* hand);

TGMMoveList* AllPossibleMovesForAllTeamPlayers(TGMGame* game, bool team1)
{
	TGMMoveList* allMoves = CreateMoveList();
	
	// we don't want to use the actual hand (because that would give insight into the player)
	// so by having a hand with just a joker, we can generate all possible moves - and we shouldn't
	// have any of the same moves
	TGMCardList handWithOnlyJoker;
	InitializeCardList(&handWithOnlyJoker);
	TGMCard* joker = CreateCard(-1, CardNumber::Joker, CardSuit::Joker);
	CardListTransfer(&handWithOnlyJoker, joker);
	
	for (int i = 0; i < kPlayers; i++)
	{
		TGMPlayer* player = GameGetPlayerAtIndex(game, i);
		
		if (PlayerIsTeam1(player) != team1)
		{
			continue;
		}
		
		TGMMoveList playerMoves;
		InitializeMoveList(&playerMoves);
		MovesForPlayer(&playerMoves, player, &handWithOnlyJoker, nullptr, GameGetBoard(game), joker, game);
		
		{
			TGMMove* move = playerMoves.first;
			while (move != nullptr)
			{
				TGMMove* next = move->nextMove;
				TransferMoveFromList(move, &playerMoves, allMoves);
				move = next;
			}
		}
		
		ReleaseMoveList(&playerMoves);
	}
	
	return allMoves;
}

void MovesForPlayerSimple(
	TGMMoveList *moveList,
	TGMPlayer* player,
	TGMGame *game,
	TGMCard* card)
{
	MovesForPlayer(moveList, player, PlayerGetHand(player), nullptr, GameGetBoard(game), card, game);
}

void MovesForPlayer(
	TGMMoveList* moveList,
	TGMPlayer* player,
	TGMCardList* hand, 
	TGMTeammates* teammates,
	TGMBoard* board, 
	TGMCard* card, 
	TGMGame* game)
{
	PlayerColor pc = PlayerGetColor(player);
	
	{
		StartingMovesForPlayer(moveList, pc, PlayerUnusedMarbleCount(player), hand, board);
		{
			TGMMove* move = moveList->first;
			while (move != nullptr)
			{
				TGMMove* next = move->nextMove;
				if (!IsMoveValidOnBoard(move, board, pc))
				{
					MoveListRemove(moveList, move);
				}
				
				move = next;
			}
		}
	}
	
	// if no moves, look at all team moves
	if (MoveListCount(moveList) == 0)
	{
		// defer creation of teammates array until needed - but only if nullptr
		// passing in empty teammate list (or any other teammate list) means we skip this part
		if (teammates == nullptr)
		{
			teammates = GameTeammatesForPlayer(game, PlayerGetColor(player));
		}
		
		// need to go find team moves
		for (int32_t p = 0; p < TeammatesCount(teammates); p++)
		{
			TGMPlayer* teammate = TeammatesPlayer(teammates, p);
			PlayerColor tc = PlayerGetColor(teammate);
			TGMMoveList teammateMoves;
			InitializeMoveList(&teammateMoves);
			StartingMovesForPlayer(&teammateMoves, tc, PlayerUnusedMarbleCount(teammate), hand, board);
			
			{
				TGMMove* move = teammateMoves.first;
				while (move != nullptr)
				{
					TGMMove* next = move->nextMove;
					if (IsMoveValidOnBoard(move, board, tc))
					{
						TransferMoveFromList(move, &teammateMoves, moveList);
					}
					
					move = next;
				}
			}
			
			ReleaseMoveList(&teammateMoves);
		}
	}

	if (card != nullptr)
	{
		{
			TGMMove* move = moveList->first;
			while (move != nullptr)
			{
				TGMMove* next = move->nextMove;
				if (!AreCardsEqual(move->card, card))
				{
					MoveListRemove(moveList, move);
				}
				
				move = next;
			}
		}
	}
	
	dassert(moveList != nullptr);
	if (MoveListCount(moveList) == 0)
	{
		if (card == nullptr)
		{
			GenerateDiscardMovesForPlayer(moveList, pc, hand);
		}
		else
		{
			TGMMove* discardMove = MakeMove(card, nullptr, true, pc, 0, 0, 0, 0, false);
			MoveListTransfer(moveList, discardMove);
		}
	}

	dassert(MoveListCount(moveList) > 0 || CardListCount(hand) == 0);
	DeallocateTeammates(teammates);
}

void GenerateDiscardMovesForPlayer(
	TGMMoveList* moveList,
	PlayerColor pc, 
	TGMCardList* hand)
{
	TGMCard *card = FirstCardNoRef(hand);
	while (card != nullptr)
	{
		TGMMove* discardMove = MakeMove(card, nullptr, true, pc, 0, 0, 0, 0, false);
		MoveListTransfer(moveList, discardMove);
		card = card->nextCard;
	}
	
	return;
}

TGMMove* BestDiscardMoveInGameFromMoves(
	TGMMoveList *discardMoves)
{
	// TODO : Improvement : Figure out which cards would be the most useful to other players
	// if they have get outs and rank those higher
	// this is a really naiive comparison today
	TGMMove* discardMove = nullptr;
	
	{
		TGMMove* move = discardMoves->first;
		while (move != nullptr)
		{
			dassert(move->isDiscard);
			if (discardMove == nullptr)
			{
				discardMove = move;
			}
			else if (CompareCardsForDiscard(move->card, discardMove->card) == TGMCardComparisonResult::Ascending)
			{
				discardMove = move;
			}
			
			move = move->nextMove;
		}
	}
	
	return RetainMove(discardMove);
}

TGMMove* BestMoveFromMoves(
	TGMMoveList* moves, 
	TGMGame* game, 
	PlayerColor pc,
	Strategy strategy)
{
	dassert(MoveListCount(moves) > 0);
	
	TGMMoveList* allPossibleOpponentMoves = nullptr;
	TGMMove* choiceMove = nullptr;
	int bestScore = 0;
	
	{
		TGMMove* move = moves->first;
		while (move != nullptr) [[likely]]
		{
			int score = CalculateWeightOfMoveInGame(move, game, pc, strategy, &allPossibleOpponentMoves);
			if (move == moves->first || bestScore < score)
			{
				bestScore = score;
				choiceMove = move;
			}
			
			move = move->nextMove;
		}
	}
	
	if (allPossibleOpponentMoves != nullptr)
	{
		ReleaseMoveList(allPossibleOpponentMoves);
	}
	
	// TODO : Improvement : look at cards to see if home moves could be made better (i.e. we have
	// cards to get home & don't use them for other things)
	// (ranker improvement?)
	
#ifdef DEBUG
	// make sure all moves are discard moves if we've found one.
	if (choiceMove && choiceMove->isDiscard)
	{
		TGMMove* move = moves->first;
		while (move != nullptr)
		{
			dassert(move->isDiscard);
			move = move->nextMove;
		}
	}
#endif
	
	return RetainMove(choiceMove);
}

void StartingMovesForPlayer(
	TGMMoveList *moveList,
	PlayerColor pc,
	int unusedMarbles, 
	TGMCardList* hand, 
	TGMBoard* board)
{
	// Builds all possible moves we could make on our own marbles, including get outs
	dassert(board != nullptr);
	dassert(hand != nullptr);

	TGMCard *card = FirstCardNoRef(hand);
	while (card != nullptr)
	{
		CardNumber cardNum = card->_number;
	
		// we do something tricky here in order to not have to write the same code twice to handle a Joker
		// we will just do one iteration of a for-loop if it's not a joker
		CardNumber min = cardNum;
		CardNumber max = cardNum;
		if (cardNum == CardNumber::Joker)
		{
			min = CardNumber::MinNumbered;
			max = static_cast<CardNumber>(static_cast<int>(CardNumber::Max) - 1);
		}
	
		// for each spot we have, add a new move
		int possSpot = -1;
		do
		{
			// always increase possSpot (which is why we start at -1 so we pick up the first marble)
			possSpot = BoardFindSpotWithPlayerColor(board, pc, (possSpot + 1));
		
			if (possSpot == -1)
				break;
		
			TGMMarble* marble = BoardMarbleAtSpot(board, possSpot);
			PlayerColor pcmc = PlayerColorForMarbleColor(marble->color);
			int normalizedOldSpot = NormalizeSpot(pc, possSpot);
		
			// first add a move to the normal point (possSpot + card value)
			for (CardNumber num = min; num <= max; IterateCardNumber(num))
			{
				dassert(IsNumberedCard(num));

				int moves = CardMoves(num);
				int newSpot = WrapSpot(possSpot + moves);
			
				{
					TGMMove* move = MakeMoveUnfiltered(card, marble, pc, cardNum, moves, possSpot, newSpot, normalizedOldSpot, 0, unusedMarbles);
					if (move)
					{
						MoveListTransfer(moveList, move);
					}
				}
			
				// 0 is a special spot -- if we haven't been around the board already,
				// we may have two choices - one to move into the final spots, one to not.
				// since we've already generated the forward move, we want to generate a get-in move
				if (normalizedOldSpot == 0)
				{
					// note that we need to subtract 1 from moves here in most places because the
					// final spots are 0 based
				
					// ignore cards that make us go out of the final spots
					// negative moves aren't final spot moves
					if (moves > kMarblesPerPlayer || moves <= 0)
						continue;
				
					int normalizedNewSpot = FinalSpotToSpot(NormalizeSpot(pcmc, WrapSpot(possSpot + moves - 1)));
				
					{
						TGMMove* move = MakeMove(card, marble, false, pc, possSpot, normalizedNewSpot, moves, 0, false);
						MoveListTransfer(moveList, move);
					}
				}
			}
		
			// now check to see if it's a point spot & handle that as well
			if (IsPointSpot(possSpot))
			{
				for (CardNumber num = min; num <= max; IterateCardNumber(num))
				{
					dassert(IsNumberedCard(num));
				
					int moves = CardMoves(num);
				
					int maxJumps = kPlayers;
					if (moves > 0) // forward progress
					{
						maxJumps -= ((NormalizeSpot(pc, possSpot) + kPlayerSpots - kPlayerPointPosition) / kPlayerSpots);
					}
					else
					{
						// TODO : handle -4 better to make sure we can't go backwards too much
						//NSLog(@"CASE NOT HANDLED : MOVE.M - 4 BACKWARDS TOO MUCH?");
						//maxJumps -= ((NormalizeSpot(color, possSpot) + kPlayerSpots - kPlayerPointPosition) / kPlayerSpots);
					}
				
					for (int jumps = 1; jumps <= maxJumps; jumps++)
					{
						int nextPointPoss;
						int remainingMoves;
					
						if (moves > 0)
						{
							nextPointPoss = possSpot + kPlayerSpots * jumps;
							remainingMoves = moves - jumps;
						}
						else
						{
							nextPointPoss = possSpot - kPlayerSpots * jumps;
							remainingMoves = moves + jumps;
						}
					
						int newSpot = WrapSpot(nextPointPoss + remainingMoves);
					
						{
							TGMMove* move = MakeMoveUnfiltered(card, marble, pc, num, moves, possSpot, newSpot, normalizedOldSpot, jumps, unusedMarbles);
							if (move != nullptr)
							{
								MoveListTransfer(moveList, move);
							}
						}
					
						if (remainingMoves == 0)
							break;
					}
				}
			}
		
			ReleaseMarble(marble);
		}
		while (possSpot + 1 < kTotalSpots);
	
		// look at all final spots
		for (int finalSpot = 0; finalSpot < kMarblesPerPlayer; finalSpot++)
		{
			if (BoardMarbleColorInFinalSpot(board, finalSpot, pc) != MarbleColor::None)
			{
				for (CardNumber num = min; num <= max; IterateCardNumber(num))
				{
					dassert(IsNumberedCard(num));
				
					int moves = CardMoves(num);
				
					// ignore backwards moves
					if (moves < 0)
						continue;
				
					int newFinalSpot = finalSpot + moves;
					if (newFinalSpot < kMarblesPerPlayer)
					{
						TGMMove* move = MakeMove(card, nullptr, false, pc, FinalSpotToSpot(finalSpot), FinalSpotToSpot(newFinalSpot), moves, 0, false);
						MoveListTransfer(moveList, move);
					}
				}
			}
		}
	
		// if it's a get out card, add a move
		if (unusedMarbles > 0)
		{
			if ((cardNum == CardNumber::King || cardNum == CardNumber::Joker || cardNum == CardNumber::Ace))
			{
				TGMMove* move = MakeMove(card, nullptr, false, pc, kGetOutSpot, PlayerStartingSpot(pc), 0, 0, false);
				MoveListTransfer(moveList, move);
			}
		}
		
		// do next card...
		card = card->nextCard;
	}
}
