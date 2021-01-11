#pragma once

enum class Strategy
{
	Human = 300,
	Passive = 301,
	Aggressive = 302,
	PassiveAggressive = 303, // randomly decides to be passive or aggressive on every play
	Adaptive1 = 304, // turns from passive to aggressive once there are less than kStrategyAdaptive1Threshhold marbles not in home spots on the other team
	Adaptive2 = 305, // turns from aggressive to passive once there are less than kStrategyAdaptive2Threshhold marbles not in home spots on the other team
	DefensivePassive = 306,
	DefensiveAggressive = 307,

	None = 299,
	Min = 300,
	Max = 307
};

static inline Strategy NextStrategy(Strategy strategy)
{
	int next = static_cast<int>(strategy) + 1;
	if (next > static_cast<int>(Strategy::Max))
		next = static_cast<int>(Strategy::Min);
	return static_cast<Strategy>(next);
}

#define IterateStrategy(x) (x = (static_cast<Strategy>((static_cast<int>(x)) + 1)))
#define IsValidStrategy(x) ((x) >= Strategy::Min && (x) <= Strategy::Max)
#define IsComputerStrategy(x) ((x) != Strategy::Human)
#define IsAggressiveStrategy(strategy) ((strategy) == Strategy::Aggressive || (strategy) == Strategy::DefensiveAggressive)
#define IsPassiveStrategy(strategy) ((strategy) == Strategy::Passive || (strategy) == Strategy::DefensivePassive)
#define IsDefensiveStrategy(strategy) ((strategy) == Strategy::DefensivePassive || (strategy) == Strategy::DefensiveAggressive)


