//
// Prefix header for all source files of the 'TGMarbles' target in the 'TGMarbles' project
//

#ifdef __OBJC__
#elif defined(TARGET_APPLE)
#define TARGET_OS_OSX 1
#endif

#if WIN32
#include <Windows.h>
#else
	#ifdef __OBJC__
		#import <Foundation/Foundation.h>
		#if TARGET_OS_IPHONE
			#import <UIKit/UIKit.h>
		#elif TARGET_OS_OSX
			#import <Cocoa/Cocoa.h>
		#else
			#error Unknown platform
		#endif
	#endif
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
#include <fstream>
#include <iostream>
#include <chrono>  
#include <time.h>
#include <vector>
#include <future>
#endif

