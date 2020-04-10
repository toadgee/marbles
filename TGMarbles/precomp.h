//
// Prefix header for all source files of the 'TGMarbles' target in the 'TGMarbles' project
//

#ifdef __OBJC__
	#import <Cocoa/Cocoa.h>
#elif defined(TARGET_APPLE)
#define TARGET_OS_OSX 1
#endif

#if WIN32
#include <Windows.h>
#elif TARGET_OS_OSX || TARGET_OS_IPHONE
#else
#error Unknown platform
#endif

#if __cplusplus
#include <sstream>
#include <memory>
#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <assert.h>
#include <stdint.h>
#include <thread>
#include <iostream>
#include <chrono>  
#include <time.h>
#endif

