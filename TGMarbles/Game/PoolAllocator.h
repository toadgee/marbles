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
	void* _object;
	TGMPoolAllocator* _next;
	TGMPoolAllocator* _previous;

#ifdef DEBUG
	int8_t _used;
	int8_t unused1;
	int16_t unused2;
	int32_t unused3;
#endif

};

TGMPoolAllocator* AllocatePoolObject(size_t size, TGMPoolAllocator** unusedFirst, TGMPoolAllocator** usedFirst);
void DeallocatePoolObject(TGMPoolAllocator* allocator, TGMPoolAllocator** unusedFirst, TGMPoolAllocator** usedFirst);
