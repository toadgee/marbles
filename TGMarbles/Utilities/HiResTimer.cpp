#include "precomp.h"
#include "HiResTimer.h"
#include <time.h>
#include <stdlib.h>
#include <string.h> // for memset

uint64_t wall_clock()
{
#if WIN32
	LARGE_INTEGER freq;
	if (!QueryPerformanceFrequency(&freq))
	{
		return 0;
	}

	LARGE_INTEGER time;
	if (!QueryPerformanceCounter(&time))
	{
		return 0;
	}

	double wallTimeDouble{ (double)(time.QuadPart / freq.QuadPart) };
	return (uint64_t)(wallTimeDouble * 1000000);
#elif TARGET_OS_OSX || TARGET_OS_IPHONE
	return mach_absolute_time();
#endif
}

static inline uint64_t internalElapsed(uint64_t start, bool cpuTime, TGHiResTimer* timer)
{
	uint64_t end;
	if (cpuTime)
		end = clock();
	else
		end = wall_clock();
	uint64_t diff = end - start;
	
	uint64_t elapsedNanoseconds;
	if (cpuTime)
	{
#if TARGET_OS_OSX || TARGET_OS_IPHONE
		elapsedNanoseconds = diff;
#elif WIN32
		elapsedNanoseconds = diff * 1000;
#else
#error not implemented
#endif
	}
	else
	{
#if TARGET_OS_OSX || TARGET_OS_IPHONE
		elapsedNanoseconds = diff * (timer->_timeBase.numer / timer->_timeBase.denom);
#elif WIN32
		elapsedNanoseconds = diff;
#else
#error missing implementation
#endif
	}
	
	return elapsedNanoseconds;
}

TGHiResTimer* CreateHiResTimer(bool cpuTime, bool start)
{
	TGHiResTimer* timer = static_cast<TGHiResTimer *>(malloc(sizeof(TGHiResTimer)));
	timer->_cpuTime = cpuTime;
	timer->_started = false;
	timer->_start = 0;
	timer->_total = 0;
	timer->_starts = 0;

#if TARGET_OS_OSX || TARGET_OS_IPHONE
	mach_timebase_info(&timer->_timeBase);
#endif

	if (start)
	{
		HiResTimerStart(timer);
	}
	
	return timer;
}

void DestroyHiResTimer(TGHiResTimer* timer)
{
#ifdef DEBUG
	memset(timer, static_cast<int>(0xDEADBEEF), sizeof(TGHiResTimer));
#endif
	free(timer);	
}

void HiResTimerStart(TGHiResTimer* timer)
{
	timer->_started = true;
	if (timer->_cpuTime)
		timer->_start = clock();
	else
		timer->_start = wall_clock();
	timer->_starts++;
}

uint64_t HiResTimerElapsed(TGHiResTimer* timer)
{
	return internalElapsed(timer->_start, timer->_cpuTime, timer);
}

uint64_t HiResTimerStop(TGHiResTimer* timer)
{
	uint64_t elapsed = internalElapsed(timer->_start, timer->_cpuTime, timer);
	timer->_total += elapsed;
	timer->_started = false;
	return elapsed;
}

double HiResTimerTotalSeconds(TGHiResTimer* timer)
{
	double seconds = static_cast<double>(timer->_total);
	if (timer->_cpuTime)
		seconds = (seconds / 1000000.0);
	else
		seconds = (seconds / 1000000000.0);
	return seconds;
}

uint64_t HiResTimerTotalMilliseconds(TGHiResTimer* timer)
{
	return HiResTimerElapsedToMilliseconds(timer, timer->_total);
}

void HiResTimerReset(TGHiResTimer* timer)
{
	timer->_started = false;
	timer->_total = 0;
	timer->_starts = 0;
}

uint64_t HiResTimerElapsedToMilliseconds(TGHiResTimer* timer, uint64_t elapsed)
{
	if (timer->_cpuTime)
		return elapsed / 1000;
	else
		return elapsed / 1000000;
}
