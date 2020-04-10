//
//	TGMViewController.h
//	MarblesMobile
//
//	Created by Todd on 10/17/12.
//	Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MessageUI/MFMailComposeViewController.h>
#import "CardView.h"
#import "BoardView.h"
#import "HandView.h"

@interface TGMViewController : UIViewController
	<MFMailComposeViewControllerDelegate>
{
	IBOutlet UIButton *_settingsButton;
	IBOutlet TGMBoardView *_boardView;
	IBOutlet TGMCardView *_discardView;
	IBOutlet TGMHandView *_handView;
	IBOutlet UIScrollView *_boardScrollView;
	IBOutlet UIView *_thisPlayerColorView;
	IBOutlet UIView *_actionView;
	IBOutlet UIButton *_fastForwardButton;
	IBOutlet UIButton *_playButton;
	IBOutlet UILabel *_scoreLabel;
}

-(IBAction)passRestOfHand:(id)sender;
-(IBAction)showFeedback:(id)sender;
-(IBAction)playMove:(id)sender;

@end
