//
//	TGMViewController.m
//	MarblesMobile
//
//	Created by Todd on 10/17/12.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import "precomp.h"
#import "DebugAssert.h"
#import "DataExtensions.h"
#import "ViewController.h"
#import "OptionsViewController.h"
#import "RandomPlayerColor.h"
#import "TouchPlayer.h"
#import "MoveList.h"
#import "UIImageExtensions.h"
#import "GameLog.h"
#import "MoveGenerator.h"
#import "Teammates.h"

// ++ HACK
#import "Game.h"
#import "ComputerPlayer.h"
#import "Options.h"

#define kNoAnimation 0
#define kDoneAnimating 1

void ViewController_PlayerPlayedMove(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move);
void ViewController_TurnStarted(void* context, TGMGame* game, TGMPlayer* player);
void ViewController_TurnEnded(void* context, TGMGame* game, TGMPlayer* player);
void ViewController_HandStarted(void* context, TGMGame* game);
void ViewController_GameStarted(void* context, TGMGame* game);
void ViewController_GameEnded(void* context, TGMGame* game, BOOL team1Won);
void ViewController_PlayerDiscarded(void* context, TGMGame* game, TGMPlayer* player, TGMCard* card);

// -- HACK

@interface TGMViewController () <UIScrollViewDelegate, TGMBoardViewDelegate, TGMHandViewDelegate>
{
	int _spot1;
	int _spot2;
	TGMMoveList *_currentValidMoves;
	NSConditionLock *_animationLock;
}
@property (readwrite, assign, nonatomic, getter=isAutomatedPlay) BOOL automatedPlay;
@property (readwrite, assign, nonatomic) TGMPlayer *player;
@property (readwrite, assign, nonatomic) TGMMoveList *currentValidMoves;
@property (readwrite, strong, nonatomic) TGMCardView *movingCardView;

@end

@implementation TGMViewController

- (void)dealloc
{
	[self setCurrentValidMoves:NULL];
}

- (void)viewDidLoad
{
	_automatedPlay = [[TGMOptions sharedInstance] isAutomatedPlay];
	
	[super viewDidLoad];
	_spot1 = -1;
	_spot2 = -1;
	
	[_discardView setBackgroundColor:[[UIColor alloc] initWithRed:0.0f green:0.0f blue:0.0f alpha:0.6f]];
	[_discardView setName:@"Discard view"];
	
	[_handView setDelegate:self];
	
	UIImage *settingsImage = [UIImage imageNamed:@"Settings"];
	[_settingsButton setTitle:@"" forState:UIControlStateNormal];
	[_settingsButton addTarget:self action:@selector(showSettings:) forControlEvents:UIControlEventTouchUpInside];
	[_settingsButton setImage:settingsImage forState:UIControlStateNormal];
	[_settingsButton setShowsTouchWhenHighlighted:YES];
	
	_animationLock = [[NSConditionLock alloc] initWithCondition:kNoAnimation];
	[_boardScrollView setDelegate:self];
	
	NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
	[center addObserver:self selector:@selector(orientationDidChange:) name:UIDeviceOrientationDidChangeNotification object:nil];
	[center addObserver:self selector:@selector(optionsLeftHandedChanged:) name:OptionsChangedIsLeftHanded object:nil];
	[center addObserver:self selector:@selector(optionsAutomatedPlayChanged:) name:OptionsChangedIsAutomatedPlay object:nil];
	[self setBackground];
	[self kickOffGameInBackground];
	[super viewDidLoad];
}

- (void)viewDidAppear:(BOOL)animated
{
	[self optionsLeftHandedChanged:nil];
	[self optionsAutomatedPlayChanged:nil];
	[super viewDidAppear:animated];
}

-(IBAction)playMove:(id)sender
{
	[self touchedDiscard];
}

-(IBAction)showSettings:(id)sender
{
	TGMOptionsViewController *ovc = [TGMOptionsViewController new];
	[self presentViewController:ovc animated:YES completion:nil];
}

 - (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView
 {
	return _boardView;
}

-(IBAction)showFeedback:(id)sender
{
	if (![MFMailComposeViewController canSendMail])
	{
		UIAlertController *controller = [UIAlertController alertControllerWithTitle:@"Sorry" message:@"Please setup e-mail on your device to send feedback" preferredStyle:UIAlertControllerStyleAlert];
		[controller addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:NULL]];
		[self presentViewController:controller animated:YES completion:NULL];
		return;
	}
	
	// generate unique report
	NSString *uuidString = [[NSUUID UUID] UUIDString];
	NSString* filename = [NSString stringWithFormat:@"GameBoard-%@.marbles", uuidString];
	NSString* subject = [NSString stringWithFormat:@"Marbles game feedback %@", uuidString];
	
	// game data
	NSData* gameData = GameLogData(GameGetGameLog([_boardView game]));
	
	// show e-mail
	MFMailComposeViewController* mail = [MFMailComposeViewController new];
	[mail setMailComposeDelegate:self];
	[mail setToRecipients:@[@"toadgee@gmail.com"]];
	[mail setMessageBody:@"[Please enter your comments about why you are sending feedback]\r\n\r\n" isHTML:NO];
	[mail setSubject:subject];
	[mail addAttachmentData:gameData mimeType:@"application/octet-stream" fileName:filename];
	
	[self presentViewController:mail animated:YES completion:nil];
}

-(void)mailComposeController:(MFMailComposeViewController*)mailController didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error
{
	[mailController dismissViewControllerAnimated:YES completion:NULL];
	
	if (result == MFMailComposeResultFailed)
	{
		UIAlertController *controller = [UIAlertController alertControllerWithTitle:@"Feedback could not be sent" message:[error description] preferredStyle:UIAlertControllerStyleAlert];
		[controller addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:NULL]];
		[self presentViewController:controller animated:YES completion:NULL];
	}
}

-(IBAction)passRestOfHand:(id)sender
{
	[_fastForwardButton setHidden:YES];
	TouchPlayerSetAutomaticallyDiscardWhenItsOurTurn([self player], YES);
}

-(NSUInteger)supportedInterfaceOrientations
{
	return UIInterfaceOrientationMaskLandscape;
}

-(void)orientationDidChange:(NSNotification *)notification
{
	[self setBackground];
}

-(void)optionsLeftHandedChanged:(NSNotification *)notification
{
	CGRect boardRect = CGRectZero;
	CGRect actionRect = CGRectZero;
	
	CGRect actionViewBounds = [_actionView bounds];
	CGRect boardViewBounds = [_boardView bounds];
	
	BOOL leftHanded = [TGMOptions sharedInstance].isLeftHanded;
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		if (leftHanded)
		{
			actionRect = CGRectMake(0, 0, actionViewBounds.size.width, actionViewBounds.size.height);
			boardRect = CGRectMake(actionViewBounds.size.width, 0, boardViewBounds.size.width, boardViewBounds.size.height);
		}
		else
		{
			boardRect = CGRectMake(0, 0, boardViewBounds.size.width, boardViewBounds.size.height);
			actionRect = CGRectMake(boardViewBounds.size.width, 0, actionViewBounds.size.width, actionViewBounds.size.height);
		}
	}
	else
	{
		boardRect = [_boardScrollView bounds];
		if (leftHanded)
		{
			actionRect = CGRectMake(0, 0, actionViewBounds.size.width, actionViewBounds.size.height);
		}
		else
		{
			actionRect = CGRectMake(
				[[self view] bounds].size.width - actionViewBounds.size.width,
				0,
				actionViewBounds.size.width,
				actionViewBounds.size.height);
		}
	}
	
	void (^animationBlock)(void) = ^
	{
		[self->_boardView setFrame:boardRect];
		[self->_actionView setFrame:actionRect];
		[self->_handView setNeedsLayout];
		[self->_handView layoutIfNeeded];
	};
	
	if (notification == nil)
	{
		// not animated, i.e. on startup
		animationBlock();
	}
	else
	{
		[UIView animateWithDuration:0.5 animations:animationBlock];
	}
}

-(void)optionsAutomatedPlayChanged:(NSNotification *)notification
{
	[self setAutomatedPlay:[[TGMOptions sharedInstance] isAutomatedPlay]];
}

#define kPanelWidth 30
#define kPanelStagger kPanelWidth
#define kPanelImage @"WoodTile.png"
- (void)setBackground
{
	UIImage *woodTile = [UIImage imageNamed:kPanelImage];
	dassert(woodTile != nil); // if we have COMBINE_HIDPI_IMAGES on, this is broken
	NSMutableArray *panels = [NSMutableArray array];
	
	CGFloat panelHeight = [woodTile size].height;
	CGFloat xPos = 0.0;
	while (xPos < [woodTile size].width)
	{
		UIImage *panel = [woodTile subimageWithRect:CGRectMake(xPos, 0, kPanelWidth, panelHeight)];
		[panels addObject:panel];
		xPos += kPanelWidth;
	}
	
	CGSize size = CGSizeMake(2048, 2048); // self.view.bounds.size; // TODO
	
	UIGraphicsBeginImageContext(size);
	NSUInteger currentPanel = 0;
	CGFloat offset = 0.0;
	CGFloat x = 0.0f;
	while (x < size.width)
	{
		CGFloat y = 0.0f;
		while (y < size.height)
		{
			UIImage *panel = [panels objectAtIndex:currentPanel++];
			currentPanel %= [panels count];
			
			[panel drawInRect:CGRectMake(x, y - offset, kPanelWidth, panelHeight)];
			
			y += panelHeight;
		}
		
		offset += kPanelStagger;
		x += kPanelWidth;
	}
	
	UIImage *panelImage = UIGraphicsGetImageFromCurrentImageContext();
	UIGraphicsEndImageContext();
	
	UIColor *boardColor = [[UIColor alloc] initWithPatternImage:panelImage];
	[[self view] setBackgroundColor:boardColor];
}

#pragma mark -
#pragma mark Properties
-(void)setCurrentValidMoves:(struct TGMMoveListStruct *)currentValidMoves
{
	if (_currentValidMoves != currentValidMoves)
	{
		ReleaseMoveList(_currentValidMoves);
		_currentValidMoves = RetainMoveList(currentValidMoves);
	}
}

-(TGMMoveList*)currentValidMoves
{
	return _currentValidMoves;
}

- (BOOL)hasTouchPlayer
{
	// working under the assumption that computer players have no context...
	return PlayerGetContext([self player]) != NULL;
}

- (void)removeMovingCardView
{
	[[self movingCardView] removeFromSuperview];
	[self setMovingCardView:nil];
}

#pragma mark -
#pragma mark CardView delegate methods

-(void)selectedCard:(TGMCardView *)cardView
{
	if (_discardView == cardView)
	{
		[self touchedDiscard];
	}
	else
	{
		dassert(false);
	}
}

-(void)touchedDiscard
{
	if (![self hasTouchPlayer]) return;
	
	if (!TouchPlayerIsMyTurn([self player]))
	{
		return;
	}
	
	if ([_handView selectedCardView] != nil)
	{
		TGMMove* move = [self makeMoveWithCard:[[_handView selectedCardView] card]];
		TouchPlayerSetMove([self player], move);
		if (move)
		{
			ReleaseMove(move);
		}
		
		TouchPlayerFinishedPlaying([self player]);
	}
	else
	{
		UIAlertController *controller = [UIAlertController alertControllerWithTitle:@"Select a card" message:@"Select a card to play or discard" preferredStyle:UIAlertControllerStyleAlert];
		[controller addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:NULL]];
		[self presentViewController:controller animated:YES completion:NULL];
	}
}

#pragma mark -
#pragma mark HandView delegate methods
-(void)selectedCardChanged:(TGMHandView *)sender
{
	[self updateBoardWithValidMoves:NO automaticallyChoose:([_handView selectedCardView] != nil)];
}

#pragma mark -
#pragma mark BoardView delegate methods
-(void)boardView:(TGMBoardView *)sender touchedSpot:(TGMTouchSpot *)spot eventType:(BoardViewEventType)event
{
	switch (event)
	{
		case BoardViewEventType_Spot1Cleared:
			[self updateBoardViewWithValidStartingMoves:NO automaticallyChoose:NO];
			break;
		case BoardViewEventType_Spot1Added:
		case BoardViewEventType_Spot1Changed:
		case BoardViewEventType_Spot2Cleared:
			[self updateBoardViewWithValidEndingMoves:NO];
			break;
		case BoardViewEventType_Spot2Changed:
		case BoardViewEventType_Spot2Added:
			[_boardView setValidTouchSpots:nil];
			break;
		default:
			dassert(false);
			break;
	}
}

// ++ HACK
- (void)kickOffGameInBackground
{
	TGMGame *game = CreateGame(NULL, NULL);
	
	static dispatch_once_t s_once;
	static PlayerColor color1;
	static PlayerColor color2;
	static PlayerColor color3;
	static PlayerColor color4;
	static PlayerColor color5;
	static PlayerColor color6;
	dispatch_once(&s_once, ^{
	
		// only generate the colors once per start of the game
		GenerateRandomColors(GameGetRandomNumberGenerator(game), &color1, &color2, &color3, &color4, &color5, &color6);
	});

	TGMPlayer* computerPlayer;
	
	computerPlayer = CreateComputerPlayer("Aubrey", Strategy::DefensiveAggressive, color1);
	GameAddPlayer(game, computerPlayer);
	ReleasePlayer(computerPlayer);
	
	computerPlayer = CreateComputerPlayer("Jason", Strategy::DefensiveAggressive, color2);
	GameAddPlayer(game, computerPlayer);
	ReleasePlayer(computerPlayer);
	
	computerPlayer = CreateComputerPlayer("Katie", Strategy::DefensiveAggressive, color3);
	GameAddPlayer(game, computerPlayer);
	ReleasePlayer(computerPlayer);
	
	computerPlayer = CreateComputerPlayer("Ryan" , Strategy::DefensiveAggressive, color4);
	GameAddPlayer(game, computerPlayer);
	ReleasePlayer(computerPlayer);
	
	computerPlayer = CreateComputerPlayer("Shauna", Strategy::DefensiveAggressive, color5);
	GameAddPlayer(game, computerPlayer);
	ReleasePlayer(computerPlayer);
	
	if ([self isAutomatedPlay])
	{
		computerPlayer = CreateComputerPlayer("Todd", Strategy::DefensiveAggressive, color6);
		[self setPlayer:computerPlayer];
	}
	else
	{
		[self setPlayer:CreateTouchPlayer("Todd", color6, game)];
	}
	
	GameAddPlayer(game, [self player]);
	
	GameSetCallbackContext(game, (__bridge void*)self);
	GameSetOnPlayerDidPlay(game, &ViewController_PlayerPlayedMove);
	GameSetOnStartedTurn(game, &ViewController_TurnStarted);
	GameSetOnEndedTurn(game, &ViewController_TurnEnded);
	GameSetOnStartedHand(game, &ViewController_HandStarted);
	GameSetOnDidStart(game, &ViewController_GameStarted);
	GameSetOnDidFinish(game, &ViewController_GameEnded);
	GameSetOnPlayerDiscarded(game, &ViewController_PlayerDiscarded);
	
	[_boardView setGame:game];
	
	[self performSelectorInBackground:@selector(startNewGame) withObject:nil];
}

- (void)startNewGame
{
	GameStartNew([_boardView game]);
}

-(void)gameStarted:(TGMGame*)game
{
	dispatch_sync(dispatch_get_main_queue(), ^
	{
		UIColor *color = [self->_boardView colorForPlayerColor:PlayerGetColor([self player])];
		[self->_thisPlayerColorView setBackgroundColor:color];
		[self->_thisPlayerColorView setHidden:NO];
		[self->_boardView setNeedsDisplay];
		[self->_handView setSelectedCardView:nil];
		[self updateScoreInGame:game];
		
		// TODO : determine if win is likely
		//BOOL winLikely = [self wouldComputerHaveWonGame:game];
		//NSLog(@"Win Likely = %@", winLikely ? @"YES" : @"NO");
	});
}

-(void)gameFinished:(TGMGame*)game team1Won:(BOOL)team1Won
{
	dispatch_sync(dispatch_get_main_queue(), ^
	{
		if ([self hasTouchPlayer])
		{
			NSString *title = @"You lost :(";
			if (PlayerIsTeam1([self player]) == team1Won)
			{
				title = @"You won! :)";
			}
			
			uint8_t team1Score = GameGetTeam1Score(game);
			uint8_t team2Score = GameGetTeam2Score(game);
			
			uint8_t higherScore = (team1Score > team2Score) ? team1Score : team2Score;
			uint8_t lowerScore = (team1Score > team2Score) ? team2Score : team1Score;
			
			UIAlertController *controller = [UIAlertController alertControllerWithTitle:title message:[NSString stringWithFormat:@"Score : %u to %u", higherScore, lowerScore] preferredStyle:UIAlertControllerStyleAlert];
			[controller addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action)
			{
				[self kickOffGameInBackground];
			}]];
			
			[self presentViewController:controller animated:YES completion:NULL];
		}
		else
		{
			[self kickOffGameInBackground];
		}
	});
}

- (void)animateCard:(TGMCard *)card fromCardViewToDiscard:(TGMCardView *)cardView completion:(void (^ __nullable)(void))completion
{
	// hide the original cardview as we're going to put something on top of it
	CGRect newCardFrame = [[self view] convertRect:[cardView frame] fromView:_handView];
	[cardView setHidden:YES];
	[self animateCard:card fromRectToDiscard:newCardFrame completion:completion];
}

- (void)animateCard:(TGMCard *)card fromRectToDiscard:(CGRect)newCardFrame completion:(void (^ __nullable)(void))completion
{
	TGMCardView *movingCardView = [[TGMCardView alloc] initWithFrame:newCardFrame];
	[self setMovingCardView:movingCardView];
	[movingCardView setCard:card];
	[movingCardView setName:@"temporary"];
	[movingCardView setUserInteractionEnabled:NO];
	[[self view] addSubview:movingCardView];
	
	[UIView animateWithDuration:[TGMOptions sharedInstance].injectedSleep animations:^{
		[movingCardView setFrame:[self->_discardView frame]];
	} completion:^(BOOL finished) {
		
		completion();
	}];
	
}

-(void)handStartedInGame:(TGMGame*)game
{
	if ([self hasTouchPlayer])
	{
		TouchPlayerSetAutomaticallyDiscardWhenItsOurTurn([self player], NO);
		[self updateFastForwardButton:game];
	}
	
	[self updateHand];
}

-(void)updateScoreInGame:(TGMGame *)game
{
	void (^updateScoreBlock)(void) = ^{
		uint8_t playerScore = GameGetTeam1Score(game);
		uint8_t otherScore = GameGetTeam2Score(game);
		if (!PlayerIsTeam1([self player]))
		{
			uint8_t tmp = playerScore;
			playerScore = otherScore;
			otherScore = tmp;
		}
		
		[self->_scoreLabel setText:[NSString stringWithFormat:@"%d - %d", playerScore, otherScore]];
	};
	
	if ([NSThread isMainThread])
	{
		updateScoreBlock();
	}
	else
	{
		dispatch_sync(dispatch_get_main_queue(), updateScoreBlock);
	}
}

-(void)updateHand
{
	void (^updateHandBlock)(void) = ^
	{
		[self->_handView setHand:PlayerGetHand([self player])];
	};
	
	if ([NSThread isMainThread])
	{
		dispatch_async(dispatch_get_main_queue(), updateHandBlock);
	}
	else
	{
		dispatch_sync(dispatch_get_main_queue(), updateHandBlock);
	}
}

-(void)turnStartedInGame:(TGMGame*)game forPlayer:(TGMPlayer*)player
{
	if ([self hasTouchPlayer])
	{
		dispatch_sync(dispatch_get_main_queue(), ^
		{
			BOOL isThisPlayer = (player == [self player]);
			if (isThisPlayer)
			{
				// change play button to discard if we only have discard moves
				{
					BOOL onlyDiscardMoves = (CardListCount(PlayerGetHand([self player])) != 0); // don't need to show pass if we don't have any more cards in our hand
					TGMMoveList* allMoves = MovesForPlayerSimple(player, game, nullptr);
					TGMMove *mv = allMoves->first;
					while (mv != nullptr)
					{
						onlyDiscardMoves = mv->isDiscard;
						if (!onlyDiscardMoves)
						{
							break;
						}
						
						mv = mv->nextMove;
					}
					ReleaseMoveList(allMoves);
					
					if (onlyDiscardMoves)
					{
						[self->_playButton setTitle:@"Discard" forState:UIControlStateNormal];
						[self->_playButton setTitle:@"Discard" forState:UIControlStateDisabled];
					}
					else
					{
						[self->_playButton setTitle:@"Play" forState:UIControlStateNormal];
						[self->_playButton setTitle:@"Play" forState:UIControlStateDisabled];
					}
				}
				
				[self->_playButton setEnabled:YES];
				[self->_playButton setHidden:NO];
			
				TGMMoveList* moves = MovesForPlayerSimple([self player], game, NULL);
				[self setCurrentValidMoves:moves];
				ReleaseMoveList(moves);
				
				[self updateBoardWithValidMoves:YES automaticallyChoose:YES];
			}
			
			[self->_boardView setNeedsDisplay];
		});
	}
}

-(void)turnEndedInGame:(TGMGame*)game forPlayer:(TGMPlayer*)player
{
	if ([self hasTouchPlayer])
	{
		[self updateFastForwardButton:game];
	
		dispatch_sync(dispatch_get_main_queue(), ^
		{
			[self->_playButton setEnabled:NO];
			[self->_playButton setHidden:YES];;
			if (player == [self player])
			{
				[self->_boardView setValidTouchSpots:nil];
				[self->_boardView clearHighlights];
			
				[self->_handView setSelectedCardView:nil];
			}
		});
	}
	else
	{
		dispatch_sync(dispatch_get_main_queue(), ^
		{
			[self->_boardView setNeedsDisplay];
		});
	}
	
	[self updateHand];
	[self updateScoreInGame:game];
}

-(void)player:(TGMPlayer*)player playedMove:(TGMMove*)move inGame:(TGMGame*)game
{
	BOOL isThisPlayer = (player == [self player]);
	if ([self hasTouchPlayer])
	{
		dispatch_sync(dispatch_get_main_queue(), ^
		{
			void (^completionBlock)(void) = ^{
				[self->_discardView setHidden:NO];
				[self->_discardView setCard:move->card];
				[self removeMovingCardView];
				[self->_boardView setNeedsDisplay];
				
				if (isThisPlayer)
				{
					[self updateHand];
				}
				
				[self->_animationLock unlockWithCondition:kDoneAnimating];
			};
			
			[self->_animationLock lock];
			
			if (isThisPlayer)
			{
				[self animateCard:move->card fromCardViewToDiscard:[self->_handView selectedCardView] completion:completionBlock];
			}
			else
			{
				CGPoint playerPoint = [self->_boardView pointForPlayerColor:PlayerGetColor(player)];
				playerPoint = [[self view] convertPoint:playerPoint fromView:self->_boardView];
				CGSize cardSize = [self->_discardView frame].size;
				CGRect playerRect = CGRectMake(playerPoint.x, playerPoint.y, cardSize.width, cardSize.height);
				playerRect = CGRectOffset(playerRect, -1 * cardSize.width / 2, -1 * cardSize.height / 2);
				[self animateCard:move->card fromRectToDiscard:playerRect completion:completionBlock];
			}
		});
	}
	
	// need to wait for animation
	[_animationLock lockWhenCondition:kDoneAnimating];
	[_animationLock unlockWithCondition:kNoAnimation];
}

-(void)player:(TGMPlayer*)player discarded:(TGMCard*)card inGame:(TGMGame*)game
{
//	dispatch_sync(dispatch_get_main_queue(), ^
//	{
//		
//		_discardView.card = card;
//		
//		[self removeMovingCardView];
//	});
	
	[self updateHand];
}

// -- HACK

-(TGMMove *)makeMoveWithCard:(TGMCard *)card
{
	TGMTouchSpot *oldSpot = [_boardView spot1];
	TGMTouchSpot *newSpot = [_boardView spot2];
	
	// filter out invalid touch moves -- ones that won't be filtered out later
	if (card == nil) return nil;							   // must play a card
	if (oldSpot != nil && newSpot == nil)
	{
		NSArray<TGMTouchSpot *> *spots = [_boardView validTouchSpots];
		if ([spots count] == 1)
		{
			newSpot = [spots firstObject];
		}
	}
	
	if (oldSpot != nil && newSpot == nil) return nil;          // don't support no target destination
	if ([newSpot isHomeSpot]) return nil;						   // don't support moving into home
	if ([oldSpot isHomeSpot] && [newSpot isFinalSpot]) return nil; // don't support moving from home to final
	
	PlayerColor pc = PlayerGetColor([self player]);
	if (oldSpot != nil && [oldSpot playerColor] != PlayerColor::None)
	{
		pc = [oldSpot playerColor];
	}
	
	TGMMove* move = nil;
	if (oldSpot == nil)
	{
		// discard
		move = MakeMove(card, nil, YES, PlayerGetColor([self player]), 0, 0, 0, 0, NO);
	}
	else if ([oldSpot isHomeSpot])
	{
		// get out
		move = MakeMove(card, nil, NO, pc, kGetOutSpot, [newSpot spot], 0, 0, NO);
	}
	else
	{
		TGMGame* game = [_boardView game];
		TGMBoard* board = GameGetBoard(game);
		TGMMarble* marble = nil;
		if (!oldSpot.isFinalSpot)
		{
			marble = BoardMarbleAtSpot(board, [oldSpot spot]);
		}
		
		// we don't need to really figure these out because the lookup pattern ignores these fields
		int jumps = 0;
		int moves = 1; // can't be 0 :)
		BOOL wentBehindHome = NO;
		int oldSpotSpot = [oldSpot spot];
		// TODO : Clean this up and remove isFinalSpot
		if ([oldSpot isFinalSpot])
		{
			oldSpotSpot = FinalSpotToSpot([oldSpot spot]);
		}
		int newSpotSpot = [newSpot spot];
		if ([newSpot isFinalSpot])
		{
			newSpotSpot = FinalSpotToSpot([newSpot spot]);
		}
		move = MakeMove(card, marble, NO, pc, oldSpotSpot, newSpotSpot, moves, jumps, wentBehindHome);
		if (marble)
		{
			ReleaseMarble(marble);
		}
	}
	
	return move;
}

- (void)updateBoardWithValidMoves:(BOOL)forceMyTurn automaticallyChoose:(BOOL)automaticallyChoose
{
	// only one of these will take effect
	[self updateBoardViewWithValidStartingMoves:forceMyTurn automaticallyChoose:automaticallyChoose];
	[self updateBoardViewWithValidEndingMoves:forceMyTurn];
}

-(void)updateBoardViewWithValidStartingMoves:(BOOL)forceMyTurn automaticallyChoose:(BOOL)automaticallyChoose
{
	if (!TouchPlayerIsMyTurn([self player]) && !forceMyTurn)
	{
		[_boardView setValidTouchSpots:nil];
		return;
	}
	
	NSMutableArray<TGMTouchSpot *> *spots = [NSMutableArray arrayWithCapacity:MoveListCount([self currentValidMoves])];
	TGMMove *mv = [self currentValidMoves]->first;
	while (mv != nullptr)
	{
		if (self->_handView.selectedCardView.card == NULL || mv->card == self->_handView.selectedCardView.card)
		{
			TGMTouchSpot *spot = [TGMTouchSpot touchSpotFromMoveBegin:mv];
			
			// we don't want to add the same spot multiple times
			for (TGMTouchSpot* sp in spots)
			{
				if ([sp isEqualToSpot:spot])
				{
					spot = nil;
					break;
				}
			}
		
			if (spot)
			{
				[spots addObject:spot];
			}
		}
		
		mv = mv->nextMove;
	};
	
	[_boardView setValidTouchSpots:spots];
	if ([spots count] == 1 && automaticallyChoose)
	{
		[_boardView highlightSpot1:[spots firstObject]];
	}
}

-(void)updateBoardViewWithValidEndingMoves:(BOOL)forceMyTurn
{
	TGMTouchSpot *spot1 = _boardView.spot1;
	if (spot1 == nil) return;
	
	if (!TouchPlayerIsMyTurn([self player]) && !forceMyTurn)
	{
		[_boardView setValidTouchSpots:nil];
		return;
	}
	
	NSMutableArray<TGMTouchSpot *> *spots = [NSMutableArray arrayWithCapacity:MoveListCount([self currentValidMoves])];
	TGMMove *mv = [self currentValidMoves]->first;
	while (mv != nullptr)
	{
		if ([[TGMTouchSpot touchSpotFromMoveBegin:mv] isEqualToSpot:spot1] && mv->card == [[self->_handView selectedCardView] card])
		{
			TGMTouchSpot *spot = [TGMTouchSpot touchSpotFromMoveEnd:mv];
			
			// we don't want to add the same spot multiple times
			for (TGMTouchSpot* sp in spots)
			{
				if ([sp isEqualToSpot:spot])
				{
					spot = nil;
					break;
				}
			}
			
			if (spot)
			{
				[spots addObject:spot];
			}
		}
		
		mv = mv->nextMove;
	}
	
	[_boardView setValidTouchSpots:spots];
}

-(void)updateFastForwardButton:(TGMGame*)game
{
	NSAssert(IsGameThread(), @"updateFastForwardButton must be called on game thread!");
	TGMTeammates* teammates = GameTeammatesIncludingPlayer(game, PlayerGetColor([self player]));
	__block BOOL allDiscardMoves = (CardListCount(PlayerGetHand([self player])) != 0); // don't need to show pass if we don't have any more cards in our hand
	for (int32_t p = 0; p < TeammatesCount(teammates); p++)
	{
		TGMPlayer* player = TeammatesPlayer(teammates, p);
		TGMMoveList* allMoves = MovesForPlayerSimple(player, game, NULL)	;
		TGMMove *mv = allMoves->first;
		while (mv != nullptr)
		{
			allDiscardMoves = mv->isDiscard;
			if (!allDiscardMoves)
			{
				break;
			}
			
			mv = mv->nextMove;
		}
		ReleaseMoveList(allMoves);
	}
	
	dispatch_sync(dispatch_get_main_queue(), ^
	{
		[self->_fastForwardButton setEnabled:(!TouchPlayerGetAutomaticallyDiscardWhenItsOurTurn([self player]) && allDiscardMoves)];
		[self->_fastForwardButton setHidden:(!allDiscardMoves || TouchPlayerGetAutomaticallyDiscardWhenItsOurTurn([self player]))];
	});
}

/*
-(BOOL)wouldComputerHaveWonGame:(TGMGame *)game
{
	// TODO
	return NO;
}
*/

@end

#pragma mark -
#pragma mark Game hooks

void ViewController_PlayerDiscarded(void* context, TGMGame* game, TGMPlayer* player, TGMCard* card)
{
	[(__bridge TGMViewController *)context player:player discarded:card inGame:game];
}

void ViewController_PlayerPlayedMove(void* context, TGMGame* game, TGMPlayer* player, TGMMove* move)
{
	[(__bridge TGMViewController *)context player:player playedMove:move inGame:game];
}

void ViewController_TurnStarted(void* context, TGMGame* game, TGMPlayer* player)
{
	[(__bridge TGMViewController *)context turnStartedInGame:game forPlayer:player];
}

void ViewController_TurnEnded(void* context, TGMGame* game, TGMPlayer* player)
{
	[(__bridge TGMViewController *)context turnEndedInGame:game forPlayer:player];
}

void ViewController_HandStarted(void* context, TGMGame* game)
{
	[(__bridge TGMViewController *)context handStartedInGame:game];
}

void ViewController_GameStarted(void* context, TGMGame* game)
{
	[(__bridge TGMViewController *)context gameStarted:game];
}

void ViewController_GameEnded(void* context, TGMGame* game, BOOL team1Won)
{
	[(__bridge TGMViewController *)context gameFinished:game team1Won:team1Won];
}
