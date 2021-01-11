#include "precomp.h"
#include "DebugAssert.h"
#include "MarblesMem.h"
#include "Teammates.h"

TGMTeammates* AllocateTeammates(void);
void DeallocateTeammates(TGMTeammates* teammates);

static std::mutex s_lock;
static TGMPoolAllocator* s_TeammatesAllocationPoolUnusedFirst = nullptr;
static TGMPoolAllocator* s_TeammatesAllocationPoolUsedFirst = nullptr;

TGMTeammates* AllocateTeammates(void)
{
	const std::lock_guard<std::mutex> lock(s_lock);
	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMTeammates), &s_TeammatesAllocationPoolUnusedFirst, &s_TeammatesAllocationPoolUsedFirst);
	TGMTeammates* teammates = static_cast<TGMTeammates *>(allocator->_object);
	teammates->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	MemIncreaseGlobalCount(g_teammatesLiving);
	
	return teammates;
}

void DeallocateTeammates(TGMTeammates* teammates)
{
	if (!teammates) return;
	
	TGMPoolAllocator* allocator = static_cast<TGMPoolAllocator*>(teammates->_holder); // need to do this before memset...
#ifdef DEBUG
	memset(teammates, static_cast<int>(0xDEADBEEF), sizeof(TGMTeammates));
	teammates->_holder = allocator;
#endif
	
	const std::lock_guard<std::mutex> lock(s_lock);
	DeallocatePoolObject(allocator, &s_TeammatesAllocationPoolUnusedFirst, &s_TeammatesAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_teammatesLiving);
}

TGMTeammates* CreateEmptyTeammates(void)
{
	return CreateTeammates(nullptr, nullptr, nullptr);
}

TGMTeammates* CreateTeammates(
	TGMPlayer* teammate1,
	TGMPlayer* teammate2,
	TGMPlayer* ownPlayer)
{
	dassert(teammate1 != nullptr || (teammate1 == nullptr && teammate2 == nullptr && ownPlayer == nullptr));
	dassert(teammate2 != nullptr || (teammate2 == nullptr && ownPlayer == nullptr));

	TGMTeammates* teammates = AllocateTeammates();
	teammates->_teammate1 = teammate1;
	teammates->_teammate2 = teammate2;
	teammates->_ownPlayer = ownPlayer;
	
	int32_t count = 0;
	if (teammate1 != nullptr) ++count;
	if (teammate2 != nullptr) ++count;
	if (ownPlayer != nullptr) ++count;
	teammates->_count = count;
	
#ifdef TEAMMATES_MEMORY_LOGGING
	int64_t tmcc = MemIncreaseGlobalCount(g_teammatesCreateCount);
	NSLog(@"<<TEAMMATES %p (+) %lld>>", teammates, tmcc);
#endif
	
	return teammates;
}

TGMPlayer* TeammatesPlayer(TGMTeammates *teammates, int32_t i)
{
	dassert(i < teammates->_count);
	if (i == 0) return teammates->_teammate1;
	else if (i == 1) return teammates->_teammate2;
	else return teammates->_ownPlayer;
}
