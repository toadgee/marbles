#include "precomp.h"
#include "DebugAssert.h"
#include "MarblesMem.h"
#include "Teammates.h"

TGMTeammates* AllocateTeammates(void);
void DeallocateTeammates(TGMTeammates* teammates);

static TGMPoolAllocator* s_TeammatesAllocationPoolUnusedFirst = NULL;
static TGMPoolAllocator* s_TeammatesAllocationPoolUsedFirst = NULL;

TGMTeammates* AllocateTeammates(void)
{
	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMTeammates), &s_TeammatesAllocationPoolUnusedFirst, &s_TeammatesAllocationPoolUsedFirst);
	TGMTeammates* teammates = (TGMTeammates *)allocator->_object;
	teammates->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	MemIncreaseGlobalCount(g_teammatesLiving);
	
	return teammates;
}

void DeallocateTeammates(TGMTeammates* teammates)
{
	if (!teammates) return;
	
	TGMPoolAllocator* allocator = (TGMPoolAllocator*)teammates->_holder; // need to do this before memset...
#ifdef DEBUG
	memset(teammates, (int)0xDEADBEEF, sizeof(TGMTeammates));
	teammates->_holder = allocator;
#endif
	
	DeallocatePoolObject(allocator, &s_TeammatesAllocationPoolUnusedFirst, &s_TeammatesAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_teammatesLiving);
}

TGMTeammates* CreateEmptyTeammates(void)
{
	return CreateTeammates(NULL, NULL, NULL);
}

TGMTeammates* CreateTeammates(
	TGMPlayer* teammate1,
	TGMPlayer* teammate2,
	TGMPlayer* ownPlayer)
{
	dassert(teammate1 != NULL || (teammate1 == NULL && teammate2 == NULL && ownPlayer == NULL));
	dassert(teammate2 != NULL || (teammate2 == NULL && ownPlayer == NULL));

	TGMTeammates* teammates = AllocateTeammates();
	teammates->_teammate1 = teammate1;
	teammates->_teammate2 = teammate2;
	teammates->_ownPlayer = ownPlayer;
	
	int32_t count = 0;
	if (teammate1 != NULL) count++;
	if (teammate2 != NULL) count++;
	if (ownPlayer != NULL) count++;
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
