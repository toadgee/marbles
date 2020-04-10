#include "precomp.h"
#include "Random.h"

#ifdef RANDOM_LOGGING
#define RandomPointer(rnd) (rnd)
#endif

static TGRandom *s_currentRandom = nullptr;

TGRandom* CreateRandomAndSeed(void)
{
	return CreateRandomWithSeed((unsigned)time(0));
}

TGRandom* CreateRandomWithSeed(unsigned seed)
{
	TGRandom* rnd = (TGRandom*)malloc(sizeof(TGRandom));
	rnd->_calls = 0;
	rnd->_seed = seed; // we will set the seed inside of RandomDoSeed
	
#ifdef RANDOM_LOGGING
	printf("RandomDoSeed on %p, new seed is %d\n", RandomPointer(rnd), seed);
#endif
	
	s_currentRandom = rnd;
#if WIN32
	srand((unsigned)seed);
#elif TARGET_OS_OSX || TARGET_OS_IPHONE
	srandom((unsigned)seed);
#else
	#error unknown platform
#endif
	
	return rnd;
}

void DestroyRandom(TGRandom* random)
{
	if (random == NULL) return;
#ifdef DEBUG
	memset(random, (int)0xDEADBEEF, sizeof(TGRandom));
#endif
	
	if (s_currentRandom == random)
	{
		s_currentRandom = nullptr;
	}

	free(random);
}

void ResetRandom(TGRandom* rnd)
{
	assert(rnd != nullptr);

#ifdef RANDOM_LOGGING
	printf("Resetting Random on %p with seed %d (%d calls)\n", RandomPointer(rnd), rnd->_seed, rnd->_calls);
#endif

#if WIN32
	srand(rnd->_seed);
#elif TARGET_OS_OSX || TARGET_OS_IPHONE
	srandom(rnd->_seed);
#else
	#error unknown platform
#endif
	
	s_currentRandom = rnd;
}

uint32_t RandomRandom(TGRandom* rnd)
{
	assert(rnd == s_currentRandom);
	
	if (rnd == NULL)
	{
#ifdef RANDOM_LOGGING
		fprintf("RandomRandom on %p\n", RandomPointer(rnd));
#endif
		
		assert(false);
		return 0; 
	}
	
	rnd->_calls++;
	
	// we don't care about using arc4random here - because we want reproducible results - and
	// if we're truly doing random, we can't get it.
	// TODO : maybe we should reduce number of randomrandom calls?...
#if WIN32
	uint32_t retval = (uint32_t)rand();
#elif TARGET_OS_OSX || TARGET_OS_IPHONE
	uint32_t retval = (uint32_t)random();
#else
	#error unknown platform
#endif

#ifdef RANDOM_LOGGING
	printf("RandomRandom on %p (%d calls) returning %u\n", RandomPointer(rnd), rnd->_calls, retval);
#endif
	
	return retval;
}

void RestoreRandom(TGRandom* rnd)
{
	assert(rnd != nullptr);

#ifdef RANDOM_LOGGING
	printf("Restoring Random on %p with seed %d (%d calls)\n", RandomPointer(rnd), rnd->_seed, rnd->_calls);
#endif
		
#if WIN32
	srand(rnd->_seed);
#elif TARGET_OS_OSX || TARGET_OS_IPHONE
	srandom(rnd->_seed);
#else
	#error unknown platform
#endif
	for (unsigned call = 0; call < rnd->_calls; call++)
	{
#if WIN32
		rand();
#elif TARGET_OS_OSX || TARGET_OS_IPHONE
		random();
#else
	#error unknown platform
#endif
	}
	
	s_currentRandom = rnd;
}

TGRandom* CopyRandom(TGRandom* rnd)
{
	TGRandom* copy = (TGRandom*)malloc(sizeof(TGRandom));
	copy->_calls = rnd->_calls;
	copy->_seed = rnd->_seed;
	
#ifdef RANDOM_LOGGING
	printf("Copying random for %p (%d calls, %d seed) ==> %p\n", RandomPointer(rnd), rnd->_calls, rnd->_seed, RandomPointer(copy));
#endif
	
	return copy;
}
