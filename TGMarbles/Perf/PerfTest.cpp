#include "precomp.h"
#include "AITest.h"
#include "HiResTimer.h"
#include "Game.h"
#include "Logging.h"
#include "HiResTimer.h"
#include "MarblesMem.h"
#include "PerfTest.h"
#include "Descriptions.h"

void PrintPerfTestResults(
	uint64_t testCount,
	Strategy s1,
	Strategy s2,
	uint64_t totalMilliseconds,
	uint64_t games,
	uint64_t hands,
	uint64_t turns,
	uint64_t team1WonGames,
	uint64_t team2WonGames,
	uint64_t dealingTeamWonGames,
	TGRandom* rng,
	uint64_t totalKills,
	uint64_t team1KilledTeam1,
	uint64_t team1KilledTeam2,
	uint64_t team2KilledTeam1,
	uint64_t team2KilledTeam2);
void PrintPerfTestResults(
	uint64_t testCount,
	Strategy s1,
	Strategy s2,
	uint64_t totalMilliseconds,
	uint64_t games,
	uint64_t hands,
	uint64_t turns,
	uint64_t team1WonGames,
	uint64_t team2WonGames,
	uint64_t dealingTeamWonGames,
	TGRandom* rng,
	uint64_t totalKills,
	uint64_t team1KilledTeam1,
	uint64_t team1KilledTeam2,
	uint64_t team2KilledTeam1,
	uint64_t team2KilledTeam2)
{
	float averageGameMS = static_cast<float>(totalMilliseconds) / static_cast<float>(games);
	float averageHandMS = static_cast<float>(totalMilliseconds) / static_cast<float>(hands);
	float averageTurnMS = static_cast<float>(totalMilliseconds) / static_cast<float>(turns);
	
	Strategy winningStrategy = s1;
	if (team1WonGames > team2WonGames) winningStrategy = s1;
	else if (team1WonGames < team2WonGames) winningStrategy = s2;
	
	printf("\n");
	printf("TIMING\n");
	printf("Took %4.2f seconds to run the tests\n\n", static_cast<double>(totalMilliseconds) / 1000.0);
	printf("Took %4.2f milliseconds per game, %llu games\n", static_cast<double>(averageGameMS), games);
	printf("Took %4.2f milliseconds per hand, %llu hands\n", static_cast<double>(averageHandMS), hands);
	printf("Took %4.4f milliseconds per turn, %llu turns\n", static_cast<double>(averageTurnMS), turns);
	printf("\n");
	printf("STATS\n");
	printf("Average : %llu turns per hand\n", turns / hands);
	printf("Average : %llu turns per game\n", turns / games);
	printf("Average : %llu hands per game\n", hands / games);
	printf("\n");
	if (rng)
	{
		printf("RANDOM\n");
		printf("Called : %u times\n", RandomCalls(rng));
		printf("Final : %u\n", RandomRandom(rng));
		printf("\n");
	}
	printf("KILLS\n");
	printf("Total : %llu kills\n", totalKills);
	printf("Average : %llu kills per game\n", totalKills / games);
	printf("Team 1 killed Team 1 : %llu times\n", team1KilledTeam1);
	printf("Team 1 killed Team 2 : %llu times\n", team1KilledTeam2);
	printf("Team 2 killed Team 1 : %llu times\n", team2KilledTeam1);
	printf("Team 2 killed Team 2 : %llu times\n", team2KilledTeam2);
	printf("\n");
	printf("WINS\n");
	printf("Total games :    %llu\n", testCount);
	printf("Dealing team won %llu times\n", dealingTeamWonGames);
	printf("Team 1 won       %llu times\n", team1WonGames);
	printf("Team 2 won       %llu times\n", team2WonGames);
	if (winningStrategy != Strategy::None)
	{
		printf("Best Strategy : %s\n", StrategyToString(winningStrategy).c_str());
	}
	
	printf("\n");
	printf("\n");
}

void RunAllPerfTests(bool large)
{
	uint64_t testCount = 1000;
	uint64_t statusUpdates = 0;
	if (large)
	{
		testCount = 10000;
		statusUpdates = 0;
	}
	
	RunAllPerfTestsWithOptions(testCount, statusUpdates);
}

int RunAllPerfTestsWithOptions(uint64_t testCount, uint64_t statusUpdates)
{
#ifdef DEBUG
	MemorySnapshot mem = CreateMemorySnapshot();
#endif

#ifdef DEBUG
	unsigned seed = 0;
#else
	unsigned seed = 105; // do a seed to be consistent across multiple perf runs
#endif
	
	uint64_t overallTestCount = 0;
	uint64_t overallGames = 0;
	uint64_t overallHands = 0;
	uint64_t overallTurns = 0;
	uint64_t overallTeam1WonGames = 0;
	uint64_t overallTeam2WonGames = 0;
	uint64_t overallKills = 0;
	uint64_t overallTeam1KilledTeam1 = 0;
	uint64_t overallTeam1KilledTeam2 = 0;
	uint64_t overallTeam2KilledTeam1 = 0;
	uint64_t overallTeam2KilledTeam2 = 0;
	uint64_t overallDealingTeamWonGames = 0;
	TGHiResTimer* timer = CreateHiResTimer(true, true);
	
	int game = 0;
	int maxStrategies = (static_cast<int>(Strategy::Max) - static_cast<int>(Strategy::Min)); // -1 for ignoring human players, + 1 for being inclusive (== 0)
	int maxGames = maxStrategies * maxStrategies;
	for (Strategy s1 = Strategy::Min; s1 <= Strategy::Max; IterateStrategy(s1))
	{
		if (s1 == Strategy::Human) continue;
		
		for (Strategy s2 = Strategy::Min; s2 <= Strategy::Max; IterateStrategy(s2))
		{
			if (s2 == Strategy::Human) continue;
			
			std::unique_ptr<AITest> test(new AITest(s1, s2, seed));
			test->SetStatusUpdates(statusUpdates);
			
			game++;
		
			// weird logging helps with comparing
			printf("------------------------------------------------\n");
			printf("%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d\n",
				game, game, game, game, game, game, game, game, game, game, game, game,
				game, game, game, game, game, game, game, game, game, game, game, game);
			printf("------------------------------------------------\n");
			printf("TEST #%d of %d\n", game, maxGames);
			printf("------------------------------------------------\n");
			printf("%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d\n",
				game, game, game, game, game, game, game, game, game, game, game, game,
				game, game, game, game, game, game, game, game, game, game, game, game);
			printf("------------------------------------------------\n");
			printf("\n");
		
			printf("STRATEGIES\n");
			printf("Team 1 : %s\n", StrategyToString(s1).c_str());
			printf("Team 2 : %s\n", StrategyToString(s2).c_str());
			printf("\n");
		
			printf("TEST SETUP\n");
			printf("Seed : %u\n", seed);
			printf("Test Count : %llu\n", testCount);
			printf("\n");
			
			printf("RUNNING TEST\n");
			
			for (uint64_t i = 0; i < testCount; i++)
			{
				test->Run(i);
			}
		
			uint64_t totalGames = HiResTimerStarts(test->GetGameTimer());
			uint64_t totalHands = test->GetHandsTotal();
			uint64_t totalTurns = test->GetTurnsTotal();
			
			PrintPerfTestResults(testCount, s1, s2, HiResTimerTotalMilliseconds(test->GetGameTimer()), totalGames, totalHands, totalTurns, 
				test->GetTeam1WonGames(), test->GetTeam2WonGames(), test->GetDealingTeamWonGames(), nullptr, test->GetTotalKills(), test->GetTeam1KilledTeam1(),
				test->GetTeam1KilledTeam2(), test->GetTeam2KilledTeam1(), test->GetTeam2KilledTeam2());
				
			overallGames += totalGames;
			overallHands += totalHands;
			overallTurns += totalTurns;
			overallTestCount += testCount;
			overallDealingTeamWonGames += test->GetDealingTeamWonGames();
			overallTeam1WonGames += test->GetTeam1WonGames();
			overallTeam2WonGames += test->GetTeam2WonGames();
			overallKills += test->GetTotalKills();
			overallTeam1KilledTeam1 += test->GetTeam1KilledTeam1();
			overallTeam1KilledTeam2 += test->GetTeam1KilledTeam2();
			overallTeam2KilledTeam1 += test->GetTeam2KilledTeam1();
			overallTeam2KilledTeam2 += test->GetTeam2KilledTeam2();
		}
			
#ifdef DEBUG
		MemoryCheck(mem);
#endif
	}
	
	HiResTimerStop(timer);
	printf("Took %llu seconds to run ALL %llu tests\n", HiResTimerTotalMilliseconds(timer) / 1000, (static_cast<uint64_t>(game) * testCount));
	printf("\n");
	printf("\n");
	
	PrintPerfTestResults(overallTestCount, Strategy::None, Strategy::None, HiResTimerTotalMilliseconds(timer), overallGames, overallHands, overallTurns,
		overallTeam1WonGames, overallTeam2WonGames, overallDealingTeamWonGames, nullptr, overallKills, overallTeam1KilledTeam1,
		overallTeam1KilledTeam2, overallTeam2KilledTeam1, overallTeam2KilledTeam2);
	DestroyHiResTimer(timer);
	
	return maxGames;
}

int RunGames(unsigned count)
{
	std::unique_ptr<AITest> test(new AITest(Strategy::Aggressive, Strategy::Aggressive, 0));
	for (uint64_t i = 0; i < count; i++)
	{
		test->Run(i);
	}

	return 0;
}

void RunMultithreadedPerfTests()
{
	TGHiResTimer* timer = CreateHiResTimer(true, true);
	const unsigned threadsToUse = std::thread::hardware_concurrency();
	printf("Using %u threads\n", threadsToUse);

	const uint32_t maxGames = 100000;
	printf("starting tests...\n");
	std::vector<std::future<int>> futures;
	const unsigned jobsPerThread = maxGames / threadsToUse;
	for (unsigned i = 0; i < threadsToUse; ++i)
	{
		unsigned gamesForThisThread = jobsPerThread;
		if (i == 0)
		{
			gamesForThisThread += maxGames % threadsToUse;
		}

		printf("thread %u : %u games\n", i, gamesForThisThread);
		std::future<int> a = std::async(std::launch::async, RunGames, gamesForThisThread);
		futures.push_back(std::move(a));
	}

	printf("Waiting for jobs to complete...\n");
	unsigned jobsLeft = threadsToUse;
	for (std::future<int>& f : futures)
	{
		f.wait();
		printf("Waiting for %u more...\n", --jobsLeft);
	}

	uint64_t elapsed = HiResTimerStop(timer);

	printf("complete in %llu ms\n", HiResTimerElapsedToMilliseconds(timer, elapsed));
}
