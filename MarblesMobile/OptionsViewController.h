//
//	OptionsViewController.h
//	Marbles
//
//	Created by Todd on 1/7/13.
//	Copyright (c) 2013 toadgee.com. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TGMOptionsViewController : UIViewController
{
	IBOutlet UILabel* _gameSpeedLabel;
	IBOutlet UISwitch* _leftHandedSwitch;
	IBOutlet UISwitch* _automatedPlaySwitch;
	IBOutlet UISlider* _gameSpeedSlider;
	IBOutlet UIButton* _runSpeedTestButton;
}

-(IBAction)close:(id)sender;
-(IBAction)gameSpeedChanged:(id)sender;
-(IBAction)leftHandedChanged:(id)sender;
-(IBAction)automatedPlayChanged:(id)sender;
-(IBAction)runSpeedTest:(id)sender;

@end
