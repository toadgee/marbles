//
//  PoolAllocator.h
//  Marbles
//
//  Created by Todd on 6/17/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#pragma once

#define TGMPoolAllocator struct TGMPoolAllocatorStruct
struct TGMPoolAllocatorStruct
{
#ifdef DEBUG
	int8_t _used;
#endif

	void* _object;
	TGMPoolAllocator* _next;
	TGMPoolAllocator* _previous;
};

TGMPoolAllocator* AllocatePoolObject(size_t size, TGMPoolAllocator** unusedFirst, TGMPoolAllocator** usedFirst);
void DeallocatePoolObject(TGMPoolAllocator* allocator, TGMPoolAllocator** unusedFirst, TGMPoolAllocator** usedFirst);
