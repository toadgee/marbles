#include "precomp.h"
#include "DebugAssert.h"
#include "Marble.h"
#include "MarblesMem.h"

TGMMarble* AllocateMarble(void);

static std::mutex s_lock;
static TGMPoolAllocator* s_MarbleAllocationPoolUnusedFirst = nullptr;
static TGMPoolAllocator* s_MarbleAllocationPoolUsedFirst = nullptr;

TGMMarble* AllocateMarble(void)
{
	const std::lock_guard<std::mutex> lock(s_lock);
	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMMarble), &s_MarbleAllocationPoolUnusedFirst, &s_MarbleAllocationPoolUsedFirst);
	TGMMarble* marble = static_cast<TGMMarble *>(allocator->_object);
	marble->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	MemIncreaseGlobalCount(g_marblesLiving);
	
	return marble;
}

void DeallocateMarble(TGMMarble* marble)
{
	dassert(marble);
	if (!marble) return;
	
	TGMPoolAllocator* allocator = static_cast<TGMPoolAllocator*>(marble->_holder); // need to do this before memset...
	
#ifdef DEBUG
	memset(marble, static_cast<int>(0xDEADBEEF), sizeof(TGMMarble));
	marble->_retainCount = 0;
	marble->_holder = allocator;
#endif
	
	const std::lock_guard<std::mutex> lock(s_lock);
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
	if (m1 == nullptr) return false;
	if (m2 == nullptr) return false;
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

