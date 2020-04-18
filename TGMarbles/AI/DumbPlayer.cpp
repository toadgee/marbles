#include "precomp.h"
#include "DumbPlayer.h"

TGMPlayer* CreateDumbPlayer(const char* name, Strategy strategy, PlayerColor pc)
{
	TGMPlayer* player = CreatePlayer(name, strategy, pc);
	return player;
}
