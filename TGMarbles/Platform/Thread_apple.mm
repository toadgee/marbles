#include "Thread.h"

void SetThreadName(const char* name)
{
	@autoreleasepool
	{
		NSString *threadName = name ? [NSString stringWithUTF8String:name] : nil;
		[[NSThread currentThread] setName:threadName];
	}
}

