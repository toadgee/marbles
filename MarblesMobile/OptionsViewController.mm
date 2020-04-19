//
//	OptionsViewController.m
//	Marbles
//
//	Created by Todd on 1/7/13.
//	Copyright (c) 2013 toadgee.com. All rights reserved.
//

#import "precomp.h"
#import "HiResTimer.h"
#import "OptionsViewController.h"
#import "Options.h"
#import "PerfTest.h"

@implementation TGMOptionsViewController

- (instancetype)init
{
	self = [super initWithNibName:@"OptionsView" bundle:nil];
	if (self)
	{
		self.modalPresentationStyle = UIModalPresentationFormSheet;
	}
	return self;
}
- (void)viewDidLoad
{
	TGMOptions* options = [TGMOptions sharedInstance];
	[_gameSpeedSlider setValue:(float)options.injectedSleep];
	[_leftHandedSwitch setOn:options.isLeftHanded];
	[_automatedPlaySwitch setOn:options.isAutomatedPlay];
	[self updateGameSpeedLabel];
	[super viewDidLoad];
}

-(IBAction)close:(id)sender
{
	[self dismissViewControllerAnimated:YES completion:NULL];
}

-(IBAction)gameSpeedChanged:(id)sender
{
	[[TGMOptions sharedInstance] setInjectedSleep:[_gameSpeedSlider value]];
	[self updateGameSpeedLabel];
}

-(IBAction)leftHandedChanged:(id)sender
{
	[[TGMOptions sharedInstance] setLeftHanded:[_leftHandedSwitch isOn]];
}

-(IBAction)automatedPlayChanged:(id)sender
{
	[[TGMOptions sharedInstance] setAutomatedPlay:[_automatedPlaySwitch isOn]];
}

-(IBAction)runSpeedTest:(id)sender
{
	TGHiResTimer* timer = CreateHiResTimer(true, true);
	uint64_t testCount = 1000;
	int games = RunAllPerfTestsWithOptions(testCount, 0);
	HiResTimerStop(timer);
	
	NSString *message = [NSString stringWithFormat:@"Took %4.2f seconds to run ALL %llu tests", (float)HiResTimerTotalMilliseconds(timer) / 1000.0, ((uint64_t)games * testCount)];
	UIAlertController *controller = [UIAlertController alertControllerWithTitle:@"Tests finished" message:message preferredStyle:UIAlertControllerStyleAlert];
	[controller addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:NULL]];
	[self presentViewController:controller animated:YES completion:NULL];
}

-(void)updateGameSpeedLabel
{
	// TODO : this sucks, we want slow/medium/fast/instant
	_gameSpeedLabel.text = [NSString stringWithFormat:@"%f seconds", [TGMOptions sharedInstance].injectedSleep];
}

@end
