//
//  Logging.m
//  Marbles
//
//  Created by todd harris on 11/27/11.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import "Logging.h"

static NSLock *s_lock = nil;
void TGMLogGameMessage(NSString *format, ...)
{
	static dispatch_once_t s_once = 0;
	dispatch_once(&s_once, ^{
		s_lock = [[NSLock alloc] init];
	});

    va_list args;
	va_start(args, format);
    NSString* str = [[NSString alloc] initWithFormat:format arguments:args];
    va_end(args);
	
    @try
    {
		[s_lock lock];
		// TODO : write to file!
    	//NSLog(@"%@", str);
    	fprintf(stdout, "%s\n", [str UTF8String]);
	}
	@finally
	{
		[s_lock unlock];
	}
}

