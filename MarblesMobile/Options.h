//
//  Options.h
//  Marbles
//
//  Created by toddha on 1/7/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TGMOptions : NSObject

+(TGMOptions *)sharedInstance;
@property (readwrite, assign, nonatomic) NSTimeInterval injectedSleep;
@property (readwrite, assign, nonatomic, getter=isLeftHanded) BOOL leftHanded;
@property (readwrite, assign, nonatomic, getter=isAutomatedPlay) BOOL automatedPlay;
@property (readwrite, assign, nonatomic) NSTimeInterval animationSpeed;

@end

extern NSString * const OptionsChangedInjectedSleep;
extern NSString * const OptionsChangedIsLeftHanded;
extern NSString * const OptionsChangedIsAutomatedPlay;
extern NSString * const OptionsChangedAnimationSpeed;
