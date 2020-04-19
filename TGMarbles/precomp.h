//
// Prefix header for all source files of the 'TGMarbles' target in the 'TGMarbles' project
//

#ifdef __OBJC__
#elif defined(TARGET_APPLE)
#define TARGET_OS_OSX 1
#endif

#if WIN32
#include <Windows.h>
#elif TARGET_OS_IPHONE
	#ifdef __OBJC__
	#import <UIKit/UIKit.h>
	#endif
#elif TARGET_OS_OSX
	#ifdef __OBJC__
	#import <Cocoa/Cocoa.h>
	#endif
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

