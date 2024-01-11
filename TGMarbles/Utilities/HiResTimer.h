//
//  HiResTimer.h
//  Marbles
//
//  Created by todd harris on 11/27/11.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

// all are nanoseconds, divide by 1,000,000 to get milliseconds

#pragma once

#include <stdbool.h>
#if TARGET_OS_OSX || TARGET_OS_IPHONE
#include <mach/mach_time.h>
#endif

#include <stdint.h>
#define TGHiResTimer struct TGHiResTimerStruct
struct TGHiResTimerStruct
{
	uint64_t _start; // start time
	uint64_t _total; // if timer is used mulitple times and not reset, this is the total # of nanoseconds run
	uint64_t _starts;

#if TARGET_OS_OSX || TARGET_OS_IPHONE
	mach_timebase_info_data_t _timeBase;
#endif

	bool _cpuTime; // if false, uses actual time, if true uses cpu time
	bool _started;
	char _padding[6];
};

#define HiResTimerTotal(timer) timer->_total
#define HiResTimerStarts(timer) timer->_starts
#define HiResTimerStarted(timer) timer->_started

TGHiResTimer* CreateHiResTimer(bool cpuTime, bool start);
void DestroyHiResTimer(TGHiResTimer* timer);

void HiResTimerStart(TGHiResTimer* timer);
uint64_t HiResTimerStop(TGHiResTimer* timer); // returns the elapsed for just this call to start
uint64_t HiResTimerElapsed(TGHiResTimer* timer); // returns the # elapsed since the timer was started
uint64_t HiResTimerElapsedToMilliseconds(TGHiResTimer* timer, uint64_t elapsed);
uint64_t HiResTimerTotalMilliseconds(TGHiResTimer* timer);
double HiResTimerTotalSeconds(TGHiResTimer* timer);
void HiResTimerReset(TGHiResTimer* timer);

