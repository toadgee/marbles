#include "precomp.h"
#include "DebugAssert.h"
#include "Marble.h"
#include "MarblesMem.h"

TGMMarble* AllocateMarble(void);

static TGMPoolAllocator* s_MarbleAllocationPoolUnusedFirst = NULL;
static TGMPoolAllocator* s_MarbleAllocationPoolUsedFirst = NULL;

TGMMarble* AllocateMarble(void)
{
	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMMarble), &s_MarbleAllocationPoolUnusedFirst, &s_MarbleAllocationPoolUsedFirst);
	TGMMarble* marble = (TGMMarble *)allocator->_object;
	marble->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	MemIncreaseGlobalCount(g_marblesLiving);
	
	return marble;
}

void DeallocateMarble(TGMMarble* marble)
{
	dassert(marble);
	if (!marble) return;
	
	TGMPoolAllocator* allocator = (TGMPoolAllocator*)marble->_holder; // need to do this before memset...
	
#ifdef DEBUG
	memset(marble, (int)0xDEADBEEF, sizeof(TGMMarble));
	marble->_retainCount = 0;
	marble->_holder = allocator;
#endif
	
	DeallocatePoolObject(allocator, &s_MarbleAllocationPoolUnusedFirst, &s_MarbleAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_marblesLiving);
}

TGMMarble* CreateMarble(MarbleColor color, int16_t distanceFromHome, bool wentBehindHome)
{
	dassert(IsMarbleColor(color));
	
	TGMMarble* marble = AllocateMarble();
	marble->_retainCount = 1;
	marble->color = color;
	marble->distanceFromHome = distanceFromHome;
	marble->wentBehindHome = wentBehindHome;
	
#ifdef MARBLE_MEMORY_LOGGING
	int64_t mcc = MemIncreaseGlobalCount(g_marbleCreateCount);
	NSLog(@"<<MARBLE %p : %d (+) %lld>>", marble, marble->_retainCount, mcc);
#endif

	return marble;
}

TGMMarble* CopyMarble(TGMMarble* marble)
{
	TGMMarble* copy = CreateMarble(marble->color, marble->distanceFromHome, marble->wentBehindHome);
	return copy;
}

bool AreMarblesEqual(TGMMarble* m1, TGMMarble* m2)
{
	if (m1 == m2) return true;
	if (m1 == NULL) return false;
	if (m2 == NULL) return false;
	if (m1->color != m2->color) return false;
	if (m1->distanceFromHome != m2->distanceFromHome) return false;
	if (m1->wentBehindHome != m2->wentBehindHome) return false;
	
	return true;
}

#ifdef DEBUG
void RetainMarble(TGMMarble* marble)
{
	int32_t rc = MemIncreaseRetainCount(marble->_retainCount);
#ifdef MARBLE_MEMORY_LOGGING
	NSLog(@"<<MARBLE %p : %d (+)>>", marble, rc);
#else
	(void)rc;
#endif
}

void ReleaseMarble(TGMMarble* marble)
{
	int32_t rc = MemDecreaseRetainCount(marble->_retainCount);
#ifdef MARBLE_MEMORY_LOGGING
	NSLog(@"<<MARBLE %p : %d (-)>>", marble, rc);
#endif
	dassert(rc >= 0);
	if (rc == 0)
	{
		DeallocateMarble(marble);
	}
}
#endif // DEBUG

