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
		while (allocator != nullptr)
		{
			assert(allocator->_used == 1);
			allocator = allocator->_next;
		}
	}

	// check no used ones in unused list
	{
		TGMPoolAllocator* allocator = unusedFirst;
		while (allocator != nullptr)
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
	dassert(unusedFirst != nullptr);
	dassert(usedFirst != nullptr);
	
	CheckAllocationPool(*usedFirst, *unusedFirst);
	TGMPoolAllocator* current = *unusedFirst;
	if (current != nullptr)
	{
		// pop off of first
		dassert(current->_previous == nullptr);
		(*unusedFirst) = current->_next;
		if ((*unusedFirst) != nullptr)
		{
			(*unusedFirst)->_previous = nullptr;
		}
	}
	else
	{
		// create a new one in the used list
		current = static_cast<TGMPoolAllocator *>(malloc(sizeof(TGMPoolAllocator)));
		current->_object = malloc(size);
		current->_previous = nullptr;
	}
	
	MarkAllocatorUsed(current);
	
	// add to beginning of used list
	current->_next = *usedFirst;
	if (*usedFirst != nullptr)
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
	
	CheckAllocationPool(*usedFirst, *unusedFirst);
	
	{
		MarkAllocatorUnused(allocator);

		// remove from used list
		if (allocator->_previous == nullptr)
		{
			(*usedFirst) = allocator->_next;
			if (allocator->_next != nullptr)
			{
				allocator->_next->_previous = nullptr;
			}
		}
		else
		{
			allocator->_previous->_next = allocator->_next;
			if (allocator->_next != nullptr)
			{
				allocator->_next->_previous = allocator->_previous;
			}

			allocator->_previous = nullptr;
		}

#if 1
		// add to unused list
		allocator->_next = (*unusedFirst);
		if ((*unusedFirst) != nullptr)
		{
			(*unusedFirst)->_previous = allocator;
		}

		(*unusedFirst) = allocator;
#else
		// just immediately free and don't use unused list
		free(allocator->_object);
		free(allocator);
#endif
	}
}
