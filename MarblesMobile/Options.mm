//
//	Options.m
//	Marbles
//
//	Created by toddha on 1/7/13.
//	Copyright (c) 2013 toadgee.com. All rights reserved.
//

#import "precomp.h"
#import "DebugAssert.h"
#import "Options.h"

@interface TGMOptions ()
{
	NSUserDefaults *_defaults;
}
@end

@implementation TGMOptions
@dynamic leftHanded;
@dynamic automatedPlay;

+(TGMOptions *)sharedInstance
{
	static TGMOptions *s_instance = nil;
	static dispatch_once_t s_predicate;
	if (!s_instance)
	{
		dispatch_once(&s_predicate, ^
		{
			s_instance = [TGMOptions new];
		});
	}
	return s_instance;
}

-(instancetype)init
{
	self = [super init];
	if (self)
	{
		_defaults = [NSUserDefaults standardUserDefaults];
	}
	
	return self;
}

NSString * const OptionsChangedInjectedSleep = @"OptionsChangedInjectedSleep";
#define kOptionNameInjectedSleep @"InjectedSleep"
#define kOptionDefaultInjectedSleep 1.0
#define kOptionMinInjectedSleep 0.0
#define kOptionMaxInjectedSleep 100.0
-(NSTimeInterval)injectedSleep
{
	NSTimeInterval val = kOptionDefaultInjectedSleep;
	@synchronized(self)
	{
		if ([_defaults objectForKey:kOptionNameInjectedSleep] != nil)
		{
			val = [_defaults floatForKey:kOptionNameInjectedSleep];
			if (val < kOptionMinInjectedSleep || val > kOptionMaxInjectedSleep)
			{
				val = kOptionDefaultInjectedSleep;
			}
		}
	}
	
	return val;
}

-(void)setInjectedSleep:(NSTimeInterval)injectedSleep
{
	if (injectedSleep < kOptionMinInjectedSleep || injectedSleep > kOptionMaxInjectedSleep)
	{
		dassert(false);
	}
	else
	{
		@synchronized(self)
		{
			if ([_defaults floatForKey:kOptionNameInjectedSleep] != injectedSleep)
			{
				[_defaults setFloat:(float)injectedSleep forKey:kOptionNameInjectedSleep];
				[_defaults synchronize];
				[[NSNotificationCenter defaultCenter] postNotificationName:(NSString *)OptionsChangedInjectedSleep object:self];
			}
		}
	}
}

NSString * const OptionsChangedIsLeftHanded = @"OptionsChangedIsLeftHanded";
#define kOptionNameLeftHanded @"LeftHanded"
#define kOptionDefaultLeftHanded NO
-(BOOL)isLeftHanded
{
	BOOL val = kOptionDefaultLeftHanded;
	@synchronized(self)
	{
		if ([_defaults objectForKey:kOptionNameLeftHanded] != nil)
		{
			val = [_defaults boolForKey:kOptionNameLeftHanded];
		}
	}
	
	return val;
}

-(void)setIsLeftHanded:(BOOL)leftHanded
{
	@synchronized(self)
	{
		if ([_defaults boolForKey:kOptionNameLeftHanded] != leftHanded)
		{
			[_defaults setBool:leftHanded forKey:kOptionNameLeftHanded];
			[_defaults synchronize];
			[[NSNotificationCenter defaultCenter] postNotificationName:(NSString *)OptionsChangedIsLeftHanded object:self];
		}
	}
}

NSString * const OptionsChangedIsAutomatedPlay = @"OptionsChangedIsAutomatedPlay";
#define kOptionNameAutomatedPlay @"AutomatedPlay"
#define kOptionDefaultAutomatedPlay NO
static BOOL s_isAutomatedPlay = NO;
-(BOOL)isAutomatedPlay
{
	@synchronized(self)
	{
		return s_isAutomatedPlay;
	}
/*
	BOOL val = kOptionDefaultAutomatedPlay;
	@synchronized(self)
	{
		if ([_defaults objectForKey:kOptionNameAutomatedPlay] != nil)
		{
			val = [_defaults boolForKey:kOptionNameAutomatedPlay];
		}
	}
	
	return val;
*/
	return s_isAutomatedPlay;
}

-(void)setIsAutomatedPlay:(BOOL)automatedPlay
{
	@synchronized(self)
	{
/*		if ([_defaults boolForKey:kOptionNameAutomatedPlay] != automatedPlay)
		{
			[_defaults setBool:automatedPlay forKey:kOptionNameAutomatedPlay];
			[_defaults synchronize];
			[[NSNotificationCenter defaultCenter] postNotificationName:(NSString *)OptionsChangedIsAutomatedPlay object:self];
		}*/
		if (s_isAutomatedPlay != automatedPlay)
		{
			s_isAutomatedPlay = automatedPlay;
			[[NSNotificationCenter defaultCenter] postNotificationName:(NSString *)OptionsChangedIsAutomatedPlay object:self];
		}
	}
}

NSString * const OptionsChangedAnimationSpeed = @"OptionsChangedAnimationSpeed";
#define kOptionNameAnimationSpeed @"AnimationSpeed"
#define kOptionDefaultAnimationSpeed 0.2
#define kOptionMinAnimationSpeed 0.0
#define kOptionMaxAnimationSpeed 2.0
-(NSTimeInterval)animationSpeed
{
	NSTimeInterval val = kOptionDefaultAnimationSpeed;
	@synchronized(self)
	{
		if ([_defaults objectForKey:kOptionNameAnimationSpeed] != nil)
		{
			val = [_defaults floatForKey:kOptionNameAnimationSpeed];
			if (val < kOptionMinAnimationSpeed || val > kOptionMaxAnimationSpeed)
			{
				val = kOptionDefaultAnimationSpeed;
			}
		}
	}
	
	return val;
}

-(void)setAnimationSpeed:(NSTimeInterval)val
{
	if (val < kOptionMinAnimationSpeed || val > kOptionMaxAnimationSpeed)
	{
		dassert(false);
	}
	else
	{
		@synchronized(self)
		{
			if ([_defaults floatForKey:kOptionNameAnimationSpeed] != val)
			{
				[_defaults setFloat:(float)val forKey:kOptionNameAnimationSpeed];
				[_defaults synchronize];
				[[NSNotificationCenter defaultCenter] postNotificationName:(NSString *)OptionsChangedAnimationSpeed object:self];
			}
		}
	}
}


@end
