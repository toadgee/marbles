#pragma once

typedef enum
{
	Strategy_Human = 300,
	Strategy_Passive = 301,
	Strategy_Aggressive = 302,
	Strategy_PassiveAggressive = 303, // randomly decides to be passive or aggressive on every play
	Strategy_Adaptive1 = 304, // turns from passive to aggressive once there are less than kStrategyAdaptive1Threshhold marbles not in home spots on the other team
	Strategy_Adaptive2 = 305, // turns from aggressive to passive once there are less than kStrategyAdaptive2Threshhold marbles not in home spots on the other team
	Strategy_DefensivePassive = 306,
	Strategy_DefensiveAggressive = 307,

	Strategy_None = 299,
	Strategy_Min = 300,
	Strategy_Max = 307
} MarbleStrategy;

static inline MarbleStrategy NextMarbleStrategy(MarbleStrategy strategy)
{
	int next = (int)strategy + 1;
	if (next > Strategy_Max)
		next = Strategy_Min;
	return (MarbleStrategy)next;
}

#define IterateMarbleStrategy(x) (x = (static_cast<MarbleStrategy>(((int)(x)) + 1)))
#define IsValidStrategy(x) ((x) >= Strategy_Min && (x) <= Strategy_Max)
#define IsComputerStrategy(x) ((x) != Strategy_Human)
#define IsAggressiveStrategy(strategy) ((strategy) == Strategy_Aggressive || (strategy) == Strategy_DefensiveAggressive)
#define IsPassiveStrategy(strategy) ((strategy) == Strategy_Passive || (strategy) == Strategy_DefensivePassive)
#define IsDefensiveStrategy(strategy) ((strategy) == Strategy_DefensivePassive || (strategy) == Strategy_DefensiveAggressive)


