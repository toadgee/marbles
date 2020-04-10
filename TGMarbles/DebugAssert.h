#pragma once

extern int s_debugAssertsOn;

#ifdef DEBUG
#define dassert(x) if (s_debugAssertsOn != 0) assert(x)
#else
#define dassert(x)
#endif
