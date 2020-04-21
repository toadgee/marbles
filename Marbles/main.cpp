#include "precomp.h"
#include "Marbles.h"
#include "PerfTest.h"
int main(int argc, const char* argv[])
{
	bool replay{ false };
	bool perf{ false };

	int matchedOptions{ 0 };
	for (int i = 1; i < argc; ++i)
	{
		if (_strnicmp(argv[i], "--replay", 8) == 0)
		{
			++matchedOptions;
			replay = true;
		}
		else if (_strnicmp(argv[i], "--perf", 6) == 0)
		{
			++matchedOptions;
			perf = true;
		}
		else if (_strnicmp(argv[i], "--help", 6) == 0)
		{
			printf("--replay : replay game\n");
			printf("--perf : perf tests\n");
			printf("--help : help message\n");
			return 0;
		}
		else
		{
			printf("Unknown option '%s'\n", argv[i]);
			return 1;
		}
	}

	if (matchedOptions > 1)
	{
		printf("Too many arguments\n");
		return 1;
	}

	if (perf)
	{
		RunAllPerfTests();
	}
	else if (replay)
	{
		// TODO
	}
	else
	{
		RunConsoleGame();
	}

	return 0;
}