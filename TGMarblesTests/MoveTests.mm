//
//	MoveTests.m
//	Marbles
//
//	Created by Todd on 9/11/12.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#include "MoveTests.h"

#include "Board.h"
#include "Card.h"
#include "ComputerPlayer.h"
#include "Deck.h"
#include "DumbPlayer.h"
#include "Game.h"
#include "GameLog.h"
#include "Marble.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "MoveRanker.h"
#include "MoveList.h"
#include "Player.h"
#include "ReplayGame.h"

#define AssertMovesValid(moveCount, am, em) \
	XCTAssertTrue(MovesMatch(am, em, moveCount), @"Actual Moves %@\nExpectedMoves %@", MoveListDescription(am), MoveListDescription(em)); \
		ReleaseMoveList(am); \
		ReleaseMoveList(em)

BOOL s_skipLongTests = YES;
#define LongTest() if (s_skipLongTests) { XCTFail(@"Skipping test"); return; }

BOOL ValidateMovesForMove(TGMMoveList* actualMoves, TGMMove* move);
BOOL ValidateMovesForMove(TGMMoveList* actualMoves, TGMMove* move)
{
	__block TGMMove* foundMove = nil;
	MoveListIterateWithBlock(actualMoves, ^(int i, TGMMove* m)
	{
		if (AreMovesEquivalent(m, move))
		{
			foundMove = m;
		}
	});
	
	if (foundMove == nil)
		return NO;
	
	unsigned i = MoveListCount(actualMoves);
	MoveListRemove(actualMoves, foundMove);
	assert(MoveListCount(actualMoves) + 1 == i);
	return YES;
}

void FinalizeExpectedMoves(TGMMoveList* expectedMoves);
void FinalizeExpectedMoves(TGMMoveList* expectedMoves)
{
	// we need this method because when we add moves in a 1 liner, we end up with 1 extra ref count
	MoveListIterateWithBlock(expectedMoves, ^(int i, TGMMove* m)
	{
		ReleaseMove(m);
	});
}

BOOL MovesMatch(TGMMoveList* actualMoves, TGMMoveList* expectedMoves, NSUInteger moveCount);
BOOL MovesMatch(TGMMoveList* actualMoves, TGMMoveList* expectedMoves, NSUInteger moveCount)
{
	FinalizeExpectedMoves(expectedMoves); // put it here so we don't have to add it to every function :)
	
	if (MoveListCount(expectedMoves) != moveCount)
	{
		NSLog(@"Didn't generate %ld expected moves - Actual Moves %@\nExpectedMoves %@", (unsigned long)moveCount, MoveListDescription(actualMoves), MoveListDescription(expectedMoves));
		return NO;
	}
	
	if (MoveListCount(actualMoves) != moveCount)
	{
		NSLog(@"Didn't generate %ld actual   moves - Actual Moves %@\nExpectedMoves %@", (unsigned long)moveCount, MoveListDescription(actualMoves), MoveListDescription(expectedMoves));
		return NO;
	}

	actualMoves = CopyMoveList(actualMoves);
	expectedMoves = CopyMoveList(expectedMoves);
	
	__block BOOL match = YES;
	MoveListIterateWithBlock(expectedMoves, ^(int i, TGMMove* move)
	{
		if (!ValidateMovesForMove(actualMoves, move))
		{
			match = NO;
		}
	});
	match = (match && MoveListCount(actualMoves) == 0);
	
	if (match)
	{
		ClearMoveList(expectedMoves);
	}
	
	ReleaseMoveList(actualMoves);
	ReleaseMoveList(expectedMoves);
	
	return match;
}

@implementation MoveTests

-(void)setUp
{
	[super setUp];
	
#ifdef DEBUG
	_mem = CreateMemorySnapshot();
#endif
	
	_playerColor = Player_Red;
	_teammate1Color = Player_White;
	_player = CreateDumbPlayer("MoveTests", Strategy_Human, _playerColor);
	_teammate1 = CreateDumbPlayer("MoveTests-TeamMate1", Strategy_Human, _teammate1Color);
	
	_game = CreateGame(NULL, NULL);
	GameAddPlayer(_game, _player);
	GameAddPlayer(_game, _teammate1);
	_board = GameGetBoard(_game);
	
	PlayerGameStarting(_player, _game);
	PlayerStartingHand(_player);
	
	_hand = PlayerGetHand(_player);
	RetainCardList(_hand);
	
	// if we want asserts off, then do this switch
	// fortunately, we don't need it today for any tests
	// turn off asserts for the tests
	//s_debugAssertsOn = 0;
}

-(void)tearDown
{
	// this should clean out the last of the memory usages
	PlayerSetGame(_player, NULL);
	ReleasePlayer(_player);
	
	PlayerSetGame(_teammate1, NULL);
	ReleasePlayer(_teammate1);
	
	ReleaseGame(_game);
	_board = NULL;
	ReleaseCardList(_hand);
	
	// turn back on asserts
	//s_debugAssertsOn = 1;
	
#ifdef DEBUG
	s_debugAssertsOn = 0;
	MemoryCheck(_mem);
	s_debugAssertsOn = 1;
#endif
	
	// Tear-down code here.
	[super tearDown];
}

-(void)testMoveGenerationBug_marbleWhichHasGoneAroundTheBoardAndAtSpotZeroShouldGenerateFinalSpot
{
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 0, YES);
		BoardPlaceMarble(_board, marble, 0);
		ReleaseMarble(marble);
	
		TGMCard* card = CreateCard(1, CardNumber_Ace, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 0,							  FinalSpotToSpot(0), 	1, 0, NO)); // move from 0 to A
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, kGetOutSpot, 	PlayerStartingSpot(_playerColor),	0, 0, NO)); // get out move
		AssertMovesValid(2, actualMoves, expectedMoves);
	}
}

-(void)testMoveGenerationBug_marbleGoneBackFourFromHomeShouldGenerateFinalSpotMovesWithGetOut
{
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 0, NO);
		BoardPlaceMarble(_board, marble, 0);
		ReleaseMarble(marble);
	
		TGMCard* card = CreateCard(1, CardNumber_Ace, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 0,										    1,	1, 0, NO)); // move from 0 to 1
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 0,						   FinalSpotToSpot(0),  1, 0, NO)); // move from 0 to A
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, kGetOutSpot, PlayerStartingSpot(_playerColor),	0, 0, NO)); // get out move
		AssertMovesValid(3, actualMoves, expectedMoves);
	}
}

-(void)testMoveGenerationBug_marbleGoneBackFourFromHomeShouldGenerateFinalSpotMovesWith5
{
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 0, NO);
		BoardPlaceMarble(_board, marble, 0);
		ReleaseMarble(marble);
		PlayerGameStarting(_player, _game);
	
		TGMCard* card = CreateCard(1, CardNumber_5, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 0,                  5, 5, 0, NO)); // move from 0 to 5
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 0, FinalSpotToSpot(4), 5, 0, NO)); // move from 0 to E
		AssertMovesValid(2, actualMoves, expectedMoves);
	}
}

-(void)testMoveGenerationBug_marbleGoneBackFourFromHomeShouldGenerateMoves
{
	/*
	--> Turn 85 started for You
	Marbles Board
					   EB					EW					 EG					  E_				   EY					E_
					   D_					D_					 D_					  D_				   D_					DR
					   C_					C_					 C_					  C_				   C_					C_
					   B_					B_					 B_					  B_				   B_					B_
					   A_					A_					 A_					  A_				   A_					A_
	_ ______ ______ ______ __B___ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ____R
			  ^		X	B		   ^	 X	 W			^	  X	  G			 ^	   X   L		  ^		X	Y		   ^	 X	 R
	a aaaaaa a99999 999998 888888 888777 777777 766666 666665 555555 555444 444444 433333 333332 222222 222111 111111 100000 00000
	7 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 43210
			 BBB				  WWWW				   GGGG				  L LLLL				 YYYY				  RRR		  
	Your hand:
		1) 6 of Spades
		2) 9 of Diamonds
		3) Jack of Diamonds
		4) Queen of Diamonds
	Enter card to play: 2
		1) Discarding 9 of Diamonds
	*/
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), kPlayerStartingPosition, NO);
		int pss = PlayerStartingSpot(_playerColor);
		BoardPlaceMarble(_board, marble, pss);
		ReleaseMarble(marble);
		PlayerGameStarting(_player, _game);
	
		TGMCard* card = nil;
		TGMMoveList* actualMoves = nil;
		TGMMoveList* expectedMoves = nil;
	
		card = CreateCard(1, CardNumber_4, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 4, 0, -4, 0, NO)); // move from 4 to 0
		TGMMove* move = RetainMove(actualMoves->first); // save before the end!
		AssertMovesValid(1, actualMoves, expectedMoves);
	
		// move the marble back move
		GameDoMove(_game, move);
		ReleaseMove(move);
		ClearCardList(_hand);
	
		card = CreateCard(2, CardNumber_9, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 0, 9, 9, 0, NO)); // move from 0 to 9
		AssertMovesValid(1, actualMoves, expectedMoves);
	}
}

-(void)testMoveGenerationBug_lastSpotBeforeFinalSpotsDoesntGenerateCorrectMovementCount
{
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), kGetInSpot, YES);
		BoardPlaceMarble(_board, marble, 0);
		ReleaseMarble(marble);
		marble->distanceFromHome = kGetInSpot;
		marble->wentBehindHome = YES;
	
		TGMCard* card = CreateCard(1, CardNumber_5, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 0, FinalSpotToSpot(4), 5, 0, NO)); // 0 to E
		AssertMovesValid(1, actualMoves, expectedMoves);
	}
}

-(void)testMoveGenerationBug_marbleGoneBackwardsOnPointGoesStrangePlaces
{
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 99, NO);
		BoardPlaceMarble(_board, marble, 99);
		ReleaseMarble(marble);
	
		TGMCard* card = CreateCard(1, CardNumber_4, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 99, 95, -4, 0, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 99, 78, -4, 1, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 99, 61, -4, 2, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 99, 44, -4, 3, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 99, 27, -4, 4, NO));
		AssertMovesValid(5, actualMoves, expectedMoves);
	}
}

-(void)testMoveGenerationBug_MovingNegativeFourFromHomePointGeneratesNoMoves
{
	/*
	Marbles Board
					   E_					E_					 E_					  E_				   E_					E_
					   D_					D_					 D_					  D_				   D_					D_
					   C_					C_					 C_					  C_				   C_					C_
					   B_					B_					 B_					  B_				   B_					B_
					   A_					A_					 A_					  A_				   A_					A_
	_ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ _R____ _____
			  ^		X	B		   ^	 X	 W			^	  X	  G			 ^	   X   L		  ^		X	Y		   ^	 X	 R
	a aaaaaa a99999 999998 888888 888777 777777 766666 666665 555555 555444 444444 433333 333332 222222 222111 111111 100000 00000
	7 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 43210
	*/
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 9, NO);
		BoardPlaceMarble(_board, marble, 9);
		ReleaseMarble(marble);
		marble->distanceFromHome = 9;
	
		TGMCard* card = CreateCard(1, CardNumber_4, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		// make sure we generate the right moves
		TGMMove* playMove = RetainMove(MakeMove(card, marble, NO, _playerColor, 9, 96, -4, 1, YES));
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9,  5, -4, 0, NO));
		MoveListAdd(expectedMoves, playMove);
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9, 79, -4, 2, YES));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9, 62, -4, 3, YES));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9, 45, -4, 4, YES));
		AssertMovesValid(5, actualMoves, expectedMoves);
	
		// move the marble back move
		GameDoMove(_game, playMove);
		ReleaseMove(playMove);
		ClearCardList(_hand);
	
		// generate the new moves we expect from that spot
		card = CreateCard(2, CardNumber_Jack, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		playMove = RetainMove(MakeMove(card, marble, NO, _playerColor, 96, 107, 11, 0, NO));
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, playMove);
		AssertMovesValid(1, actualMoves, expectedMoves);
	
		// put the marble at 107
		GameDoMove(_game, playMove);
		ReleaseMove(playMove);
		ClearCardList(_hand);
	
		// create 3 cards - of which some we don't expect moves for
		TGMCard* card5 = CreateCard(3, CardNumber_5, CardSuit_Diamonds);
		TGMCard* card6 = CreateCard(4, CardNumber_6, CardSuit_Diamonds);
		TGMCard* card7 = CreateCard(5, CardNumber_7, CardSuit_Diamonds);
		CardListAdd(_hand, card5);
		CardListAdd(_hand, card6);
		CardListAdd(_hand, card7);
		ReleaseCard(card5);
		ReleaseCard(card6);
		ReleaseCard(card7);
	
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, nil, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card5, marble, NO, _playerColor, 107, FinalSpotToSpot(3), 5, 0, YES));
		MoveListAdd(expectedMoves, MakeMove(card6, marble, NO, _playerColor, 107, FinalSpotToSpot(4), 6, 0, YES));
		AssertMovesValid(2, actualMoves, expectedMoves);
	}
}

-(void)testMoveIsValidBug_NegativeFourCanGoPastOurOwnHome
{
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 9, NO);
		BoardPlaceMarble(_board, marble, 9);
	
		TGMCard* card = CreateCard(1, CardNumber_4, CardSuit_Diamonds);
		TGMMove* move = MakeMove(card, marble, NO, _playerColor, 9, 96, -4, 0, NO);
	
		XCTAssertTrue(IsMoveValidOnBoard(move, _board, _playerColor), @"Move is not valid!");
		ReleaseMove(move);
		ReleaseCard(card);
		ReleaseMarble(marble);
	}
}

-(void)testMoveIsValidBug_NegativeFourFromLastNormalizedSpotIsInvalid
{
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 9, NO);
		BoardPlaceMarble(_board, marble, 0);
		ReleaseMarble(marble);
	
		TGMCard* card = CreateCard(1, CardNumber_4, CardSuit_Diamonds);
	
		TGMMove* move = MakeMove(card, marble, NO, _playerColor, 0, 104, -4, 0, YES);
		XCTAssertTrue(IsMoveValidOnBoard(move, _board, _playerColor), @"Move is not valid!");
	
		// test the generation worked
		CardListAdd(_hand, card);
		ReleaseCard(card);
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, move);
	
		AssertMovesValid(1, actualMoves, expectedMoves);
	}
}

-(void)testMoveIsValidBug_GetOutForTeammatesShouldBeValid
{
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 0, NO);
		TGMCard* card = CreateCard(1, CardNumber_King, CardSuit_Diamonds);
		TGMMove* move = MakeMove(card, marble, NO, _teammate1Color, kGetOutSpot, PlayerStartingSpot(_teammate1Color), 0, 0, NO);
		ReleaseCard(card);
		ReleaseMarble(marble);
	
		// make sure the teammate has 1 marble
		PlayerRestoreMarble(_teammate1, GetMarbleForPlayer(_teammate1Color, 0));
	
		// put all the marbles in the final spots
		for (int i = 0; i < kMarblesPerPlayer; i++)
		{
			MarbleColor mc = PlayerRemoveFirstUnusedMarbleColor(_player);
			BoardPlaceMarbleAtFinalSpot(_board, mc, PlayerGetColor(_player), i);
		}
	
		// turn off asserts for this check
		BOOL isValid = IsMoveValidOnBoard(move, _board, _playerColor);
		ReleaseMove(move);
		XCTAssertTrue(isValid, @"Get out move is not valid!");
	}
}

-(void)testMoveIsValidBug_MovingAcrossWrappingBoundaryNegativelyProducesDeadMarble
{
	@autoreleasepool
	{
		// Move (Red) - [-4] From 45 to 81 with 4 of Diamonds (goneAroundNow = YES, weight = 39, YES)
		// ...INVALID : Move (Red) - [2] From 81 to 83 with 2 of Hearts (goneAroundNow = YES, weight = 0, NO) : new final spot isn't a valid spot
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), -27, NO);
		BoardPlaceMarble(_board, marble, 81);
	
		TGMCard* card = CreateCard(1, CardNumber_2, CardSuit_Diamonds);
		TGMMove* move = MakeMove(card, marble, NO, _teammate1Color, 81, 83, 2, 0, NO);
		BOOL isValid = IsMoveValidOnBoard(move, _board, _playerColor);
		XCTAssertTrue(isValid, @"Move is not valid!");
		ReleaseMarble(marble);
		ReleaseMove(move);
		ReleaseCard(card);
	}
}

-(void)testMovePointGeneration
{
	@autoreleasepool
	{
		// a marble on the point should generate a certain set of moves
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 9, NO);
		BoardPlaceMarble(_board, marble, 9);
		ReleaseMarble(marble);
	
		TGMCard* card = CreateCard(1, CardNumber_Jack, CardSuit_Diamonds);
	
		CardListAdd(_hand, card);
		ReleaseCard(card);
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9,  20, 11, 0, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9,  37, 11, 1, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9,  54, 11, 2, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9,  71, 11, 3, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9,  88, 11, 4, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9, 105, 11, 5, NO));
		AssertMovesValid(6, actualMoves, expectedMoves);
	}
}

-(void)testMovePointGenerationBug_SecondPointJumpsTooFar
{
	@autoreleasepool
	{
		// a marble on the point should generate a certain set of moves
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 27, NO);
		BoardPlaceMarble(_board, marble, 27);
		ReleaseMarble(marble);
	
		TGMCard* card = CreateCard(1, CardNumber_Jack, CardSuit_Diamonds);
	
		CardListAdd(_hand, card);
		ReleaseCard(card);
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 27,	 38, 11, 0, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 27,	 55, 11, 1, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 27,	 72, 11, 2, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 27,	 89, 11, 3, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 27, 106, 11, 4, NO));
		AssertMovesValid(5, actualMoves, expectedMoves);
	}
}

-(void)testMovePointGenerationBug_PointJumpsTooFarPastGoInSpot
{
	/*--> Turn 325 started for You
	Marbles Board
					   EB					EW					 EG					  EL				   EY					ER
					   DB					DW					 DG					  DL				   DY					DR
					   CB					CW					 CG					  CL				   CY					CR
					   B_					BW					 B_					  BL				   B_					BR
					   A_					AW					 AG					  A_				   A_					A_
	_ ______ ______ ______ ______ _R____ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ _____
			  ^		X	B		   ^	 X	 W			^	  X	  G			 ^	   X   L		  ^		X	Y		   ^	 X	 R
	a aaaaaa a99999 999998 888888 888777 777777 766666 666665 555555 555444 444444 433333 333332 222222 222111 111111 100000 00000
	7 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 43210
			  BB										G					 L					  Y					   R		  
	Your hand:
		1) 2 of Spades
		2) 4 of Diamonds
		3) 4 of Spades
		4) 5 of Hearts
		5) 7 of Clubs
		6) Jack of Diamonds
	Enter card to play: 6
		1) from 81 to 92
		2) from 81 to 1 // SHOULD BE A
	*/
	
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 81, NO);
		BoardPlaceMarble(_board, marble, 81);
		ReleaseMarble(marble);

		TGMCard* card = CreateCard(1, CardNumber_Jack, CardSuit_Diamonds);

		CardListAdd(_hand, card);
		ReleaseCard(card);
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 81,                 92, 11, 0, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 81, FinalSpotToSpot(0), 11, 1, YES));
		AssertMovesValid(2, actualMoves, expectedMoves);
	}
}
-(void)testMoveGenerationBug_TwoNegativeFourPointJumpsGeneratesFinalSpotMoves
{
	@autoreleasepool
	{
		TGMCard* card = nil;
		TGMMove* playMove = nil;
		TGMMoveList* actualMoves = nil;
		TGMMoveList* expectedMoves = nil;
		const int pss = PlayerStartingSpot(_playerColor);
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 0, NO);
	
		// given the following 6 cards (and played in this order):
		//	- King
		//	- 2
		//	- 4
		//	- 4
		//	- Jack (not played)
		//	- 7 (not played)
		//	- 6 (not played)
		// we should generate the correct moves
	
		// make the marble get out
		card = CreateCard(1, CardNumber_King, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
		playMove = RetainMove(MakeMove(card, marble, NO, _playerColor, kGetOutSpot, pss, 0, 0, NO));
		ReleaseMarble(marble);
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, playMove);
		AssertMovesValid(1, actualMoves, expectedMoves);
		GameDoMove(_game, playMove);
		ClearCardList(_hand);
		marble = BoardMarbleAtSpot(_board, playMove->newSpot);
		ReleaseMove(playMove);
		XCTAssertTrue(marble->distanceFromHome == 4, @"Marble isn't 4 from home = %d", marble->distanceFromHome);
	
		// move the marble 2
		card = CreateCard(2, CardNumber_2, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
		playMove = RetainMove(MakeMove(card, marble, NO, _playerColor, 4, 6, 2, 0, NO));
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, playMove);
		AssertMovesValid(1, actualMoves, expectedMoves);
		GameDoMove(_game, playMove);
		ReleaseMove(playMove);
		ClearCardList(_hand);
		XCTAssertTrue(marble->distanceFromHome == 6, @"Marble isn't 6 from home = %d", marble->distanceFromHome);
	
		// move the marble 4
		card = CreateCard(3, CardNumber_4, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
		playMove = RetainMove(MakeMove(card, marble, NO, _playerColor, 6, 2, -4, 0, NO));
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, playMove);
		AssertMovesValid(1, actualMoves, expectedMoves);
		GameDoMove(_game, playMove);
		ReleaseMove(playMove);
		ClearCardList(_hand);
		XCTAssertTrue(marble->distanceFromHome == 2, @"Marble isn't 2 from home = %d", marble->distanceFromHome);
	
		// move the marble 4
		card = CreateCard(4, CardNumber_4, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
		playMove = RetainMove(MakeMove(card, marble, NO, _playerColor, 2, 106, -4, 0, YES));
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, playMove);
		AssertMovesValid(1, actualMoves, expectedMoves);
		GameDoMove(_game, playMove);
		ReleaseMove(playMove);
		ClearCardList(_hand);
		XCTAssertTrue(marble->distanceFromHome == 106, @"Marble isn't 106 from home = %d", marble->distanceFromHome);
	
		// see what moves are generated for a 6, 7 and a jack
		// move the marble 4
		TGMCard* card6 = CreateCard(5, CardNumber_6, CardSuit_Diamonds);
		TGMCard* card7 = CreateCard(6, CardNumber_7, CardSuit_Diamonds);
		TGMCard* cardJ = CreateCard(7, CardNumber_Jack, CardSuit_Diamonds);
		CardListAdd(_hand, card6);
		CardListAdd(_hand, card7);
		CardListAdd(_hand, cardJ);
		ReleaseCard(card6);
		ReleaseCard(card7);
		ReleaseCard(cardJ);
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, nil, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card6, marble, NO, _playerColor, 106, FinalSpotToSpot(3), 6, 0, YES));
		MoveListAdd(expectedMoves, MakeMove(card7, marble, NO, _playerColor, 106, FinalSpotToSpot(4), 7, 0, YES));
		AssertMovesValid(2, actualMoves, expectedMoves);
		
		ReleaseMarble(marble);
	}
}

-(void)testMovePointGenerationBug_TwoNegativeFourPointJumpsGoesPastHome
{
	// TODO
}

-(void)testMoveGenerationBug_PlayerPlayingTeammateInvalidPastHome
{
	// this marble moved home -> -4 -> -4 -> 2 -> [9]
	/*
					   E_					E_					 E_					  E_				   E_					ER
					   D_					D_					 D_					  D_				   D_					D_
					   C_					C_					 C_					  C_				   C_					C_
					   B_					B_					 B_					  B_				   B_					B_
					   A_					A_					 A_					  A_				   A_					A_
	_ ______ ______ ______ _B____ ______ ______ W_____ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ R____
			  ^		X	B		   ^	 X	 W			^	  X	  G			 ^	   X   L		  ^		X	Y		   ^	 X	 R
	a aaaaaa a99999 999998 888888 888777 777777 766666 666665 555555 555444 444444 433333 333332 222222 222111 111111 100000 00000
	7 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 43210
			 BBBB				  WWWW				 G GGGG				  L LLLL			   Y YYYY				  RRR		  
	</turn 29> - Current player is : [2] Jason (Yellow, Passive computer)
	<turn 30> - Current player is : [1] Shauna (Blue, Passive computer)
	...INVALID : Move (White) - [9] From 70 to 79 with 9 of Hearts (weight=uncalc, moves=9, jumps=0) : this won't fit in the final spots - it goes beyond the bounds
	*/
	@autoreleasepool
	{
		TGMMarble* whiteMarble = CreateMarble(GetMarbleForPlayer(Player_White, 0), 0, NO);
	
		BoardPlaceMarble(_board, whiteMarble, 70);
		whiteMarble->distanceFromHome = 106;
		ReleaseMarble(whiteMarble);
	
		TGMCard* card = CreateCard(1, CardNumber_9, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
		
		TGMTeammates* teammates = CreateTeammates(_teammate1, NULL, NULL);
		TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, teammates, _board, card, _game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, nil, YES, _playerColor, 0, 0, 0, 0, NO));
		AssertMovesValid(1, actualMoves, expectedMoves);
	}
}

-(void)testMoveGenerationBug_ShouldNotBeAbleToPassOwnMarblesIfGoingInToFinalSpots
{
	// 
	// ---> Turn 292 started for Ryan
	// Ryan played from 86 to C with Joker
	// Marbles Board
	//					  EB				   EW					EG					 EL					  EY				   ER
	//					  DB				   DW					DG					 DL					  DY				   DR
	//					  CB				   CW					CG					 CL					  CY				   CR
	//					  B_				   BW					BG					 BL					  BY				   B_
	//					  A_				   A_					A_					 A_					  AY				   A_
	// _ ______ ______ ____B_ __(___ ______ W_____ ______ ______ G_____ ______ ______ ______ ______ ______ ______ ______ ______ _____
	//			 ^	   X   B		  ^		X	W		   ^	 X	 G			^	  X	  L			 ^	   X   Y		  ^		X	R
	// a aaaaaa a99999 999998 888888 888777 777777 766666 666665 555555 555444 444444 433333 333332 222222 222111 111111 100000 00000
	// 7 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 43210
	//			 B																L										  RR		 
	// <-- Turn 292 ended for Ryan
	// 
	@autoreleasepool
	{
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 104, NO);
		BoardPlaceMarble(_board, marble, 104);
		ReleaseMarble(marble);
	
		TGMCard* card = CreateCard(1, CardNumber_7, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		// place them in each of the 4 spots before home and make sure none can be generated
	
		// spot 105
		{
			// we can play from this spot
			TGMMarble* marble2 = CreateMarble(GetMarbleForPlayer(_playerColor, 1), 0, NO);
			int16_t newSpot = WrapSpot(marble->distanceFromHome + 1);
			BoardPlaceMarble(_board, marble2, newSpot);
			marble2->distanceFromHome = newSpot;
		
			TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
			TGMMoveList* expectedMoves = CreateMoveList();
			MoveListAdd(expectedMoves, MakeMove(card, marble2, NO, _playerColor, newSpot, FinalSpotToSpot(3), 7, 0, YES));
			AssertMovesValid(1, actualMoves, expectedMoves);
			
			BoardRemoveMarble(_board, newSpot);
			ReleaseMarble(marble2);
		}

		// spot 106
		{
			// we can play from this spot
			TGMMarble* marble2 = CreateMarble(GetMarbleForPlayer(_playerColor, 1), 0, NO);
			int16_t newSpot = WrapSpot(marble->distanceFromHome + 2);
			BoardPlaceMarble(_board, marble2, newSpot);
			marble2->distanceFromHome = newSpot;
		
			TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
			TGMMoveList* expectedMoves = CreateMoveList();
			MoveListAdd(expectedMoves, MakeMove(card, marble2, NO, _playerColor, newSpot, FinalSpotToSpot(4), 7, 0, YES));
			AssertMovesValid(1, actualMoves, expectedMoves);
			
			BoardRemoveMarble(_board, newSpot);
			ReleaseMarble(marble2);
		}
	
		// spot 107
		{
			// no moves
			TGMMarble* marble2 = CreateMarble(GetMarbleForPlayer(_playerColor, 1), 0, NO);
			int16_t newSpot = WrapSpot(marble->distanceFromHome + 3);
			BoardPlaceMarble(_board, marble2, newSpot);
			marble2->distanceFromHome = newSpot;
		
			TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
			TGMMoveList* expectedMoves = CreateMoveList();
			MoveListAdd(expectedMoves, MakeMove(card, nil, YES, _playerColor, 0, 0, 0, 0, NO));
			AssertMovesValid(1, actualMoves, expectedMoves);
		
			BoardRemoveMarble(_board, newSpot);
			ReleaseMarble(marble2);
		}
	
		// spot 0
		{
			// no moves
			TGMMarble* marble2 = CreateMarble(GetMarbleForPlayer(_playerColor, 1), 0, NO);
			int16_t newSpot = WrapSpot(marble->distanceFromHome + 3);
			BoardPlaceMarble(_board, marble2, newSpot);
			marble2->distanceFromHome = newSpot;
		
			TGMMoveList* actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
			TGMMoveList* expectedMoves = CreateMoveList();
			MoveListAdd(expectedMoves, MakeMove(card, nil, YES, _playerColor, 0, 0, 0, 0, NO));
			AssertMovesValid(1, actualMoves, expectedMoves);
			
			BoardRemoveMarble(_board, newSpot);
			ReleaseMarble(marble2);
		}
	}
}

-(void)testMoveGenerationBug_MarbleGetsStuckAtLastSpotBeforeFinalSpots
{
	// 
	// Marbles Board
	//					  E_				   E_					E_					 E_					  E_				   E_
	//					  D_				   D_					D_					 D_					  D_				   D_
	//					  C_				   C_					C_					 C_					  C_				   C_
	//					  B_				   B_					B_					 B_					  B_				   B_
	//					  A_				   A_					A_					 A_					  A_				   A_
	// _ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ______ ____R
	//			 ^	   X   B		  ^		X	W		   ^	 X	 G			^	  X	  L			 ^	   X   Y		  ^		X	R
	// a aaaaaa a99999 999998 888888 888777 777777 766666 666665 555555 555444 444444 433333 333332 222222 222111 111111 100000 00000
	// 7 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 432109 876543 210987 654321 098765 43210
	// 
	@autoreleasepool
	{
		TGMCard* card = nil;
		TGMMove* playMove = nil;
		TGMMoveList* actualMoves = nil;
		TGMMoveList* expectedMoves = nil;
		const int pss = PlayerStartingSpot(_playerColor);
		TGMMarble* marble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 0, NO);
	
		// given the following 6 cards (and played in this order):
		//	- King (get out)
		//	- 5
		//	- 7
		//	- 7
		// then given a hand of a Jack and -4, we should play a negative 4
	
		// make the marble get out
		card = CreateCard(1, CardNumber_King, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
		playMove = RetainMove(MakeMove(card, marble, NO, _playerColor, kGetOutSpot, pss, 0, 0, NO));
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, playMove);
		AssertMovesValid(1, actualMoves, expectedMoves);
		GameDoMove(_game, playMove);
		ClearCardList(_hand);
		ReleaseMarble(marble);
		marble = BoardMarbleAtSpot(_board, playMove->newSpot);
		ReleaseMove(playMove);
		XCTAssertTrue(marble->distanceFromHome == 4, @"Marble isn't 4 from home = %d", marble->distanceFromHome);
	
		// move the marble 5
		card = CreateCard(2, CardNumber_5, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
		playMove = RetainMove(MakeMove(card, marble, NO, _playerColor, 4, 9, 5, 0, NO));
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, playMove);
		AssertMovesValid(1, actualMoves, expectedMoves);
		GameDoMove(_game, playMove);
		ReleaseMove(playMove);
		ClearCardList(_hand);
		XCTAssertTrue(marble->distanceFromHome == 9, @"Marble isn't 9 from home = %d", marble->distanceFromHome);
	
		// move the marble 7
		card = CreateCard(3, CardNumber_7, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
		playMove = RetainMove(MakeMove(card, marble, NO, _playerColor, 9, 101, 7, 5, NO));
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9, 16, 7, 0, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9, 33, 7, 1, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9, 50, 7, 2, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9, 67, 7, 3, NO));
		MoveListAdd(expectedMoves, MakeMove(card, marble, NO, _playerColor, 9, 84, 7, 4, NO));
		MoveListAdd(expectedMoves, playMove);
		AssertMovesValid(6, actualMoves, expectedMoves);
		GameDoMove(_game, playMove);
		ReleaseMove(playMove);
		ClearCardList(_hand);
		XCTAssertTrue(marble->distanceFromHome == 101, @"Marble isn't 101 from home = %d", marble->distanceFromHome);
	
		// move the marble 7
		card = CreateCard(4, CardNumber_7, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
		playMove = RetainMove(MakeMove(card, marble, NO, _playerColor, 101, 0, 7, 0, YES));
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, card, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, playMove);
		AssertMovesValid(1, actualMoves, expectedMoves);
		GameDoMove(_game, playMove);
		ReleaseMove(playMove);
		ClearCardList(_hand);
		XCTAssertTrue(marble->distanceFromHome == 0, @"Marble isn't 0 from home = %d", marble->distanceFromHome);
	
		// see what moves are generated for a Jack and -4
		TGMCard* card4 = CreateCard(5, CardNumber_4, CardSuit_Diamonds);
		TGMCard* cardJ = CreateCard(6, CardNumber_Jack, CardSuit_Diamonds);
		CardListAdd(_hand, card4);
		CardListAdd(_hand, cardJ);
		ReleaseCard(card4);
		ReleaseCard(cardJ);
		actualMoves = MovesForPlayer(_player, _hand, CreateEmptyTeammates(), _board, nil, _game);
		expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card4, marble, NO, _playerColor, 0, 104, -4, 0, YES));
		AssertMovesValid(1, actualMoves, expectedMoves);
		
		ReleaseMarble(marble);
	}
}

-(void)testMoveRankingBug_AceGetsPlayedInappropriatelyIfInFinalSpots
{
	// sort of a stupid bug here, but apparently if the computer has something like
	// _1_2_, the 2 gets moved in first before the 1, which is a much harder thing to play
	// as the Ace is more valuable, so we need to value the 1 playing over the 2.
	@autoreleasepool
	{
		PlayerColor pc = Player_Green;
		TGMPlayer* player = CreatePlayer("testMoveGenerationBug_CanPassMarblesRightBeforeHome", Strategy_Human, pc);
		TGMGame* game = CreateGame(NULL, NULL);
		GameAddPlayer(game, player);
		ReleasePlayer(player);
		TGMBoard* board = GameGetBoard(game);
		PlayerGameStarting(player, game);
		
		TGMMarble* marble1 = CreateMarble(GetMarbleForPlayer(pc, 0), 0, NO);
		BoardPlaceMarbleAtFinalSpot(board, marble1->color, pc, 3);
		
		TGMMarble* marble2 = CreateMarble(GetMarbleForPlayer(pc, 1), 0, NO);
		BoardPlaceMarbleAtFinalSpot(board, marble2->color, pc, 1);
		
		TGMCard* card = CreateCard(1, CardNumber_Ace, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		TGMMoveList* actualMoves = MovesForPlayer(player, _hand, CreateEmptyTeammates(), board, card, game);
		TGMMoveList* expectedMoves = CreateMoveList();
		TGMMove* move1 = MakeMove(card, marble1, NO, pc, FinalSpotToSpot(3), FinalSpotToSpot(4), 1, 0, NO);
		TGMMove* move2 = MakeMove(card, marble1, NO, pc, FinalSpotToSpot(1), FinalSpotToSpot(2), 1, 0, NO);
		
		for (MarbleStrategy strategy = Strategy_Min; strategy < Strategy_Max; IterateMarbleStrategy(strategy))
		{
			if (IsPassiveStrategy(strategy) || IsAggressiveStrategy(strategy))
			{
				TGMMove *m1 = CopyMove(move1);
				TGMMove *m2 = CopyMove(move2);
				int move1Score = CalculateWeightOfMoveInGame(m1, game, pc, strategy, NULL);
				int move2Score = CalculateWeightOfMoveInGame(m2, game, pc, strategy, NULL);
				XCTAssertTrue(move1Score > move2Score);
				
				ReleaseMove(m1);
				ReleaseMove(m2);
			}
		}
		
		MoveListAdd(expectedMoves, move1);
		MoveListAdd(expectedMoves, move2);
		MoveListAdd(expectedMoves, MakeMove(card, NULL, NO, pc, kGetOutSpot, PlayerStartingSpot(pc), 0, 0, NO)); // get out move
		AssertMovesValid(3, actualMoves, expectedMoves);
		
		ReleaseMarble(marble1);
		ReleaseMarble(marble2);
		
		PlayerSetGame(player, NULL); // ensure the player/game circular dependency is broken!
		ReleaseGame(game);
	}
}

-(void)testMoveGenerationBug_CanPassMarblesRightBeforeHome
{
	// we use green for this one because that's how the bug showed up
	// first marble goes at 53
	// other marble is at 48
	// we could play a 6, strangely
	@autoreleasepool
	{
		PlayerColor pc = Player_Green;
		TGMPlayer* player = CreatePlayer("testMoveGenerationBug_CanPassMarblesRightBeforeHome", Strategy_Human, pc);
		TGMGame* game = CreateGame(NULL, NULL);
		GameAddPlayer(game, player);
		ReleasePlayer(player);
		TGMBoard* board = GameGetBoard(game);
		PlayerGameStarting(player, game);
	
		TGMMarble* marble1 = CreateMarble(GetMarbleForPlayer(pc, 0), 107, NO);
		BoardPlaceMarble(board, marble1, 53);
	
		TGMMarble* marble2 = CreateMarble(GetMarbleForPlayer(pc, 1), 102, NO);
		BoardPlaceMarble(board, marble2, 48);
	
		TGMMarble* marble3 = CreateMarble(GetMarbleForPlayer(pc, 2), 0, NO);
		BoardPlaceMarbleAtFinalSpot(board, marble3->color, pc, 4);
	
		TGMCard* card = CreateCard(1, CardNumber_6, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		ReleaseCard(card);
	
		TGMMoveList* actualMoves = MovesForPlayer(player, _hand, CreateEmptyTeammates(), board, card, game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, nil, YES, pc, 0, 0, 0, 0, NO)); // discard move
		AssertMovesValid(1, actualMoves, expectedMoves);
		
		ReleaseMarble(marble1);
		ReleaseMarble(marble2);
		ReleaseMarble(marble3);
		
		PlayerSetGame(player, NULL); // ensure the player/game circular dependency is broken!
		ReleaseGame(game);
	}
}

-(void)testMoveGenerationBug_CanPassMarblesOverThe2dBoardBoundary
{
	// Basically, if we have 2 marbles - one at spot 2, and one at spot 105, we can
	// move past the one at spot 2 because asking for the next marble at spot 105 is returns
	// -1 (because we've reached the end of the board boundary). We need to make sure we continue
	// checking the board.
	@autoreleasepool
	{
		PlayerColor pc = Player_Black;
		TGMPlayer* player = CreatePlayer("testMoveGenerationBug_CanPassMarblesOverThe2dBoardBoundary", Strategy_Human, pc);
		TGMGame* game = CreateGame(NULL, NULL);
		GameAddPlayer(game, player);
		ReleasePlayer(player);
		PlayerGameStarting(player, game);
		
		TGMMarble* marble1 = CreateMarble(GetMarbleForPlayer(pc, 0), 15, NO);
		BoardPlaceMarble(GameGetBoard(game), marble1, 105);
		
		TGMMarble* marble2 = CreateMarble(GetMarbleForPlayer(pc, 1), 20, NO);
		BoardPlaceMarble(GameGetBoard(game), marble2, 2);
		
		TGMCard* card = CreateCard(1, CardNumber_Queen, CardSuit_Diamonds);
		CardListAdd(_hand, card);
		
		TGMMoveList* actualMoves = MovesForPlayer(player, _hand, CreateEmptyTeammates(), GameGetBoard(game), card, game);
		TGMMoveList* expectedMoves = CreateMoveList();
		MoveListAdd(expectedMoves, MakeMove(card, marble2, NO, pc, 2, 14, 12, 0, NO));
		
		AssertMovesValid(1, actualMoves, expectedMoves);
		
		ReleaseMarble(marble1);
		ReleaseMarble(marble2);
		ReleaseCard(card);
		
		PlayerSetGame(player, NULL); // ensure the player/game circular dependency is broken!
		ReleaseGame(game);
	}
}

-(void)testCalculateIntermediatePoints_ForwardIncludingWrapping
{
	// just test normal generation between two points on the board (including wrapping)
	@autoreleasepool
	{
		TGMMarble* unusedMarble = CreateMarble(GetMarbleForPlayer(_playerColor, 0), 0, YES);
		BOOL unusedWentBehindHome = NO;
		PlayerColor unusedPC = _playerColor;
	
		for (int startingSpot = 0; startingSpot < kTotalSpots; startingSpot++)
		{
			NSMutableArray* expectedPoints = [NSMutableArray array];
			[expectedPoints addObject:[NSNumber numberWithInt:startingSpot]];
			int spot = startingSpot;
			for (CardNumber cn = CardNumber_Min; cn <= CardNumber_Max; IterateCardNumber(cn))
			{
				if (!IsNumberedCard(cn))
				{
					continue;
				}
				else if (CardMoves(cn) < 0)
				{
					continue;
				}
			
				spot = WrapSpot(spot + 1);
				[expectedPoints addObject:[NSNumber numberWithInt:spot]];
			
				TGMCard* card = CreateCard(1, cn, CardSuit_Diamonds);
				TGMMove* move = MakeMove(card, unusedMarble, NO, unusedPC, startingSpot, WrapSpot(startingSpot + CardMoves(cn)), CardMoves(cn), 0, unusedWentBehindHome);
				MoveCalculateIntermediateSpots(move);
			
				for (unsigned i = 0; i < expectedPoints.count; i++)
				{
					int expected = ((NSNumber *)expectedPoints[i]).intValue;
					int actual = move->spots[i];
					XCTAssertTrue(expected == actual, @"Does not match at spot %d (expected %d, actual %d)", i, expected, actual);
					if (expected != actual)
					{
						return; // fail the test once!
					}
				}
			
				ReleaseCard(card);
				ReleaseMove(move);
			}
		}
	
		ReleaseMarble(unusedMarble);
	}
}

-(void)testHandSort
{
	@autoreleasepool
	{
		TGMCardList* hand = CreateCardList();
		TGMCard* cardQ = CreateCard(1, CardNumber_Queen, CardSuit_Hearts);
		TGMCard* card5 = CreateCard(2, CardNumber_5, CardSuit_Diamonds);
		TGMCard* cardK = CreateCard(2, CardNumber_King, CardSuit_Spades);
		
		CardListAdd(hand, cardQ);
		CardListAdd(hand, card5);
		CardListAdd(hand, cardK);
		
		CardListSort(hand);
		XCTAssertTrue(CardListCount(hand) == 3, @"Hand is missing cards!");
		
		XCTAssertTrue(CardAtIndex(hand, 0) == card5, @"Card 0 isn't 5!");
		XCTAssertTrue(CardAtIndex(hand, 1) == cardQ, @"Card 0 isn't Queen!");
		XCTAssertTrue(CardAtIndex(hand, 2) == cardK, @"Card 0 isn't King!");
		
		// since we asked for it from CardAtIndex
		for (int i = 0; i < 2; i++)
		{
			ReleaseCard(cardQ);
			ReleaseCard(card5);
			ReleaseCard(cardK);
		}
		
		ReleaseCardList(hand);
	}
}

-(void)testHandSortComprehensive
{
	__block BOOL failed = NO;
	
	// if this test takes a really long time, we probably are leaking decks!
	// but we expect it to pass within 10 seconds
	for (int j = 0; j < 100; j++)
	{
		@autoreleasepool
		{
			for (unsigned i = 0; i < DeckCapacity(); i++)
			{
				TGMCardList* hand = CreateCardList();
				TGMDeck *deck = CreateNonEmptyDeck();
				TGRandom* rnd = CreateRandomAndSeed();
				DeckShuffle(deck, rnd);
				
				for (int k = 0; k < 1; k++) // deck only has 108 cards.
				{
					for (unsigned cn = 0; cn < i; cn++)
					{
						TGMCard* card = DeckRemoveTopCard(deck);
						CardListAdd(hand, card);
						ReleaseCard(card);
					}
		
					CardListSort(hand);
					XCTAssertTrue(CardListCount(hand) == i, @"HAND LOST SOME CARDS IN SORT");
		
					__block TGMCard *lastCard = NULL;
					CardListIterateWithBlock(hand, ^(unsigned, TGMCard *card)
					{
						BOOL ascending = CompareCards(lastCard, card);
						XCTAssertTrue(ascending, @"%@ > %@", CardDescription(lastCard, false), CardDescription(card, false));
			
						if (lastCard) ReleaseCard(lastCard);
						lastCard = card;
						RetainCard(card);
			
						if (!ascending) failed = YES;
						return ascending;
					});
					
					if (lastCard) ReleaseCard(lastCard);
					ClearCardList(hand);
					
					if (failed) break;
				}
				
				ReleaseCardList(hand);
				ReleaseDeck(deck);
				
				if (failed) break;
			}
		}
		if (failed) break;
	}
}

-(void)testEntireGameNoLeak
{
	@autoreleasepool
	{
		// run an entire game
		TGMGame* game = CreateGame(NULL, NULL);
		MarbleStrategy strategy = Strategy_Min;
		for (unsigned p = 0; p < kPlayers; p++)
		{
			if (strategy == Strategy_Human) strategy = NextMarbleStrategy(strategy);
			PlayerColor pc = PlayerColorForPosition(p);
			TGMPlayer* player = CreateComputerPlayer(PlayerColorToString(pc), strategy, pc);
			GameAddPlayer(game, player);
			ReleasePlayer(player);
		}
		
		GameStartNew(game);
		
		ReleaseGame(game);
	}
}

-(void)testMoveFreezeUnfreezeThroughEntireGameNoLeaks
{
	@autoreleasepool
	{
		NSData* data;
		
		// memory snapshots helpful for figuring out where we leak
#ifdef DEBUG
		MemorySnapshot baseline = CreateMemorySnapshot();
#endif
		
		@autoreleasepool
		{
			TGMGame* game = CreateGame(NULL, NULL);
			MarbleStrategy strategy = Strategy_Min;
			for (unsigned p = 0; p < kPlayers; p++)
			{
				if (strategy == Strategy_Human) strategy = NextMarbleStrategy(strategy);
				PlayerColor pc = PlayerColorForPosition(p);
				TGMPlayer* player = CreateComputerPlayer(PlayerColorToString(pc), strategy, pc);
				GameAddPlayer(game, player);
				ReleasePlayer(player);
			}
			
			GameStartNew(game);
		
			TGMGameLog* gameLog = GameGetGameLog(game);
			data = GameLogData(gameLog);
			
			ReleaseGame(game);
		}
		
#ifdef DEBUG
		MemoryCheck(baseline);
#endif
		
		@autoreleasepool
		{
			TGMGameLog* gameLog = CreateGameLogFromData(data);
			TGMReplayGame* replayGame = CreateReplayGame(gameLog, nullptr);
			ReleaseGameLog(gameLog);
			ReplayGameGo(replayGame);
			
			ReleaseReplayGame(replayGame);
		}
		
#ifdef DEBUG
		MemoryCheck(baseline);
#endif
	}
}

-(void)testMoveFreezeUnfreezeThroughEntireGameWorks
{
	@autoreleasepool
	{
		// run an entire game
		TGMGame* game = CreateGame(NULL, NULL);
		MarbleStrategy strategy = Strategy_Min;
		for (unsigned p = 0; p < kPlayers; p++)
		{
			if (strategy == Strategy_Human) strategy = NextMarbleStrategy(strategy);
			PlayerColor pc = PlayerColorForPosition(p);
			
			TGMPlayer* player = CreateComputerPlayer(PlayerColorToString(pc), strategy, pc);
			GameAddPlayer(game, player);
			ReleasePlayer(player);
		}
		
		GameStartNew(game);
		
		// validate game log serialization works correctly
		TGMGameLog* gameLog = GameGetGameLog(game);
		NSData* gameLogData = GameLogData(gameLog);
		TGMGameLog* outGameLog = CreateGameLogFromData(gameLogData);
		XCTAssertTrue(AreGameLogsEqual(gameLog, outGameLog), @"Game logs aren't the same!");
		NSData* outGameLogData = GameLogData(outGameLog);
		XCTAssertTrue([gameLogData isEqualToData:outGameLogData], @"GameLog Data isn't the same!");

		// now check that if we play the exact same game back, we get the exact same results
		// note that we want the game seed to be different
		TGRandom* replayRandom = CreateRandomWithSeed(2013);
		TGMReplayGame* replayGame = CreateReplayGame(gameLog, replayRandom);
		XCTAssertTrue(RandomSeed(GameGetRandomNumberGenerator(ReplayGameGame(replayGame))) != RandomSeed(GameGetRandomNumberGenerator(game)), @"Game seeds should not be the same!");
		ReplayGameGo(replayGame);
	
		// validate games were the same
		TGMGameLog* replayGameLog = GameGetGameLog(ReplayGameGame(replayGame));
		NSData* replayGameLogData = GameLogData(replayGameLog);
		XCTAssertTrue([gameLogData isEqualToData:replayGameLogData], @"GameLog Data isn't the same as replay!");
		XCTAssertTrue(AreBoardsEqual(GameGetBoard(game), GameGetBoard(ReplayGameGame(replayGame))), @"Boards don't match!\n%@\n%@", GameGetBoard(game), GameGetBoard(ReplayGameGame(replayGame)));
		
		ReleaseGame(game);
		ReleaseReplayGame(replayGame);
		ReleaseGameLog(outGameLog);
	}
}

@end
