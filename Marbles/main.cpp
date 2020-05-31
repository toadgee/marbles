#include "precomp.h"
#include "Marbles.h"
#include "MarblesReplay.h"
#include "PerfTest.h"
int main(int argc, const char* argv[])
{
	bool replay{ false };
	bool perf{ false };
	std::string filename;

	int matchedOptions{ 0 };
	for (int i = 1; i < argc; ++i)
	{
		if (_strnicmp(argv[i], "--replay", 8) == 0)
		{
			++matchedOptions;
			replay = true;

			++i;
			if (i < argc)
			{
				filename = argv[i];
			}
		}
		else if (_strnicmp(argv[i], "--perf", 6) == 0)
		{
			++matchedOptions;
			perf = true;
		}
		else if (_strnicmp(argv[i], "--test", 6) == 0)
		{
			RunMultithreadedPerfTests();
			return 0;
		}
		else if (_strnicmp(argv[i], "--help", 6) == 0)
		{
			printf("--replay [filename]: replay game\n");
			printf("--perf : perf tests\n");
			printf("--test : multi-threaded perf tests (temporary)\n");
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
		if (filename.size() == 0)
		{
			printf("filename required. Use --help for details.\n");
			exit(1);
		}

		ReplayGame(filename);
	}
	else
	{
		RunConsoleGame();
	}

	return 0;
}