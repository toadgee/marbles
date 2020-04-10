#include "precomp.h"
#include "DebugAssert.h"
#include "PoolAllocator.h"

#ifdef DEBUG
#define MarkAllocatorUsed(allocator) allocator->_used = 1
#define MarkAllocatorUnused(allocator) allocator->_used = 0
void CheckAllocationPool(TGMPoolAllocator* usedFirst, TGMPoolAllocator* unusedFirst);
void CheckAllocationPool(TGMPoolAllocator* usedFirst, TGMPoolAllocator* unusedFirst)
{
	// this gets REALLY slow for debug builds (like 45x slower)
	static int s_count = 0;
	s_count++;
	if (s_count < 50) return;
	s_count = 0;
	
	// check no unused ones in used list
	{
		TGMPoolAllocator* allocator = usedFirst;
		while (allocator != NULL)
		{
			assert(allocator->_used == 1);
			allocator = allocator->_next;
		}
	}

	// check no used ones in unused list
	{
		TGMPoolAllocator* allocator = unusedFirst;
		while (allocator != NULL)
		{
			assert(allocator->_used == 0);
			allocator = allocator->_next;
		}
	}
}
#else
#define CheckAllocationPool(used, unused)
#define MarkAllocatorUsed(allocator)
#define MarkAllocatorUnused(allocator)
#endif

TGMPoolAllocator* AllocatePoolObject(size_t size, TGMPoolAllocator** unusedFirst, TGMPoolAllocator** usedFirst)
{
	dassert(unusedFirst != NULL);
	dassert(usedFirst != NULL);
	if (unusedFirst == NULL || usedFirst == NULL)
		return NULL;
	
	CheckAllocationPool(*usedFirst, *unusedFirst);
	TGMPoolAllocator* current = *unusedFirst;
	if (current != NULL)
	{
		// pop off of first
		dassert(current->_previous == NULL);
		(*unusedFirst) = current->_next;
		if ((*unusedFirst) != NULL)
		{
			(*unusedFirst)->_previous = NULL;
		}
	}
	else
	{
		// create a new one in the used list
		current = (TGMPoolAllocator *)malloc(sizeof(TGMPoolAllocator));
		current->_object = malloc(size);
		current->_previous = NULL;
	}
	
	MarkAllocatorUsed(current);
	
	// add to beginning of used list
	current->_next = *usedFirst;
	if (*usedFirst != NULL)
	{
		(*usedFirst)->_previous = current;
	}
	
	(*usedFirst) = current;
	
	return current;
}

void DeallocatePoolObject(TGMPoolAllocator* allocator, TGMPoolAllocator** unusedFirst, TGMPoolAllocator** usedFirst)
{
	dassert(allocator);
	dassert(unusedFirst);
	dassert(usedFirst);
	if (!allocator || !unusedFirst || !usedFirst) 
		return;
	
	CheckAllocationPool(*usedFirst, *unusedFirst);
	
	{
		MarkAllocatorUnused(allocator);

		// remove from used list
		if (allocator->_previous == NULL)
		{
			(*usedFirst) = allocator->_next;
			if (allocator->_next != NULL)
			{
				allocator->_next->_previous = NULL;
			}
		}
		else
		{
			allocator->_previous->_next = allocator->_next;
			if (allocator->_next != NULL)
			{
				allocator->_next->_previous = allocator->_previous;
			}

			allocator->_previous = NULL;
		}

		// add to unused list
		allocator->_next = (*unusedFirst);
		if ((*unusedFirst) != NULL)
		{
			(*unusedFirst)->_previous = allocator;
		}

		(*unusedFirst) = allocator;
	}
}
