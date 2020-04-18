#pragma once

enum class GameState
{
	NotStarted   = 400,
	GameStarting = 401,
	HandStarting = 402,
	TurnStarting = 403,
	Playing	   = 404,
	TurnEnding   = 405,
	HandEnding   = 406,
	GameEnding   = 407,
	GameOver	   = 408
};
