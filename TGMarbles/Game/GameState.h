#pragma once

typedef enum
{
	State_NotStarted   = 400,
	State_GameStarting = 401,
	State_HandStarting = 402,
	State_TurnStarting = 403,
	State_Playing	   = 404,
	State_TurnEnding   = 405,
	State_HandEnding   = 406,
	State_GameEnding   = 407,
	State_GameOver	   = 408
} MarbleGameState;
