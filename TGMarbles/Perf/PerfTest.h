//
//  PerfTest.h
//  Marbles
//
//  Created by todd harris on 11/27/11.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

void RunAllPerfTests(bool large, bool quiet);
int RunAllPerfTestsWithOptions(uint64_t testCount, uint64_t statusUpdates, bool printPartial);
void RunMultithreadedPerfTests();
