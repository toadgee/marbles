#include "precomp.h"
#include "DumbPlayer.h"

TGMPlayer* CreateDumbPlayer(const char* name, MarbleStrategy strategy, PlayerColor pc)
{
	TGMPlayer* player = CreatePlayer(name, strategy, pc);
	return player;
}
