//
//  Logging.h
//  Marbles
//
//  Created by todd harris on 11/27/11.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#ifdef MACOS // Logging
#import <Foundation/Foundation.h>

#ifdef __cplusplus
extern "C" {
#endif

void TGMLogGameMessage(NSString *format, ...);

#ifdef __cplusplus
}
#endif
#elif WIN32
// TODO : Win32
#define TGMLogGameMessage(...)
#endif
