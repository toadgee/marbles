#include "precomp.h"
#include "DebugAssert.h"
#include "Game.h"
#include "MarblesMem.h"
#include "Player.h"

void PlayerEventOnGameStarting(TGMPlayer* player);
void PlayerEventOnGameEnding(TGMPlayer* player);
void PlayerEventOnHandStarting(TGMPlayer* player);
void PlayerEventOnDealtCard(TGMPlayer* player, TGMCard* card);

TGMPlayer* AllocatePlayer(void);
void DeallocatePlayer(TGMPlayer* player);

static std::mutex s_lock;
static TGMPoolAllocator* s_playerAllocationPoolUnusedFirst = NULL;
static TGMPoolAllocator* s_playerAllocationPoolUsedFirst = NULL;

TGMPlayer* AllocatePlayer(void)
{
	const std::lock_guard<std::mutex> lock(s_lock);
	TGMPoolAllocator* allocator = AllocatePoolObject(sizeof(TGMPlayer), &s_playerAllocationPoolUnusedFirst, &s_playerAllocationPoolUsedFirst);
	TGMPlayer* player = (TGMPlayer *)allocator->_object;
	player->_holder = allocator; // need a back pointer to the allocator for a quick return policy
	
	MemIncreaseGlobalCount(g_playersLiving);
	
	return player;
}

void DeallocatePlayer(TGMPlayer* player)
{
	dassert(player);
	if (!player) return;
	
	if (player->_hand) ReleaseCardList(player->_hand);
	if (player->_game) ReleaseGame(player->_game);
	if (player->_onDestroy)
	{
		player->_onDestroy(player);
	}
	
	TGMPoolAllocator* allocator = (TGMPoolAllocator*)player->_holder; // need to do this before memset...
	
#ifdef DEBUG
	memset(player, (int)0xDEADBEEF, sizeof(TGMPlayer));
	player->_retainCount = 0;
	player->_holder = allocator;
#endif
	
	const std::lock_guard<std::mutex> lock(s_lock);
	DeallocatePoolObject(allocator, &s_playerAllocationPoolUnusedFirst, &s_playerAllocationPoolUsedFirst);
	MemDecreaseGlobalCount(g_playersLiving);
}

TGMPlayer* RetainPlayer(TGMPlayer* player)
{
	if (player)
	{
		LogMemoryRetain("PLAYER", player, MemIncreaseRetainCount(player->_retainCount));
	}
	
	return player;
}

void ReleasePlayer(TGMPlayer* player)
{
	if (!player) return;
	
	int32_t rc = MemDecreaseRetainCount(player->_retainCount);
	LogMemoryRelease("PLAYER", player, rc);
	
	dassert(rc >= 0);
	if (rc == 0)
	{
		DeallocatePlayer(player);
	}
}

TGMPlayer* CreatePlayer(const char* name, Strategy strategy, PlayerColor pc)
{
	dassert(IsValidStrategy(strategy));
	
	TGMPlayer* player = AllocatePlayer();
	
	player->_retainCount = 1;
	player->_name = name;
	player->_pc = pc;
	player->_strategy = strategy;
	memset(player->_marblesInHome, false, sizeof(player->_marblesInHome));
	
	player->_hand = NULL;
	player->_isTeam1 = false;
	
	player->_game = NULL;
	
	player->_onGameStarting = NULL;
	player->_onGameEnding = NULL;
	player->_onHandStarting = NULL;
	player->_onDealtCard = NULL;
	player->_onPlayInGame = NULL;
	player->_onDescription = NULL;
	player->_onDestroy = NULL;
	
	return player;
}

void PlayerCopyData(TGMPlayer* oldPlayer, TGMPlayer* newPlayer)
{
	// we don't copy _strategy, _game, or the events
	if (newPlayer->_hand) ReleaseCardList(newPlayer->_hand);
	newPlayer->_hand = oldPlayer->_hand ? CopyCardList(oldPlayer->_hand) : nullptr;
	
	newPlayer->_name = oldPlayer->_name;
	newPlayer->_pc = oldPlayer->_pc;
	newPlayer->_isTeam1 = oldPlayer->_isTeam1;
	
	for (int m = 0; m < kMarblesPerPlayer; ++m)
	{
		newPlayer->_marblesInHome[m] = oldPlayer->_marblesInHome[m];
	}
}

void PlayerSetName(TGMPlayer* player, const char* name)
{
	player->_name = name;
}

void PlayerSetIsTeam1(TGMPlayer* player, bool isTeam1)
{
	player->_isTeam1 = isTeam1;
}

void PlayerSetContext(TGMPlayer* player, void* context)
{
	player->_context = context;
}

TGMGame* PlayerGetGame(TGMPlayer* player)
{
	return RetainGame(player->_game);
}

void PlayerSetGame(TGMPlayer* player, TGMGame* game)
{
	if (player->_game != game)
	{
		ReleaseGame(player->_game);
		player->_game = RetainGame(game);
	}
}

int8_t PlayerUnusedMarbleCount(TGMPlayer* player)
{
	// TODO : Cache this
	int8_t count = 0;
	for (int i = 0; i < kMarblesPerPlayer; i++)
	{
		if (player->_marblesInHome[i])
		{
			count++;
		}
	}
	
	return count;
}

bool PlayerHasUnusedMarbleColor(TGMPlayer* player, MarbleColor mc)
{
	return player->_marblesInHome[PositionForMarble(mc)];
}

MarbleColor PlayerRemoveFirstUnusedMarbleColor(TGMPlayer* player)
{
	MarbleColor mc = MarbleColor::None;
	for (int i = 0; i < kMarblesPerPlayer; i++)
	{
		if (player->_marblesInHome[i])
		{
			mc = GetMarbleForPlayer(player->_pc, i);
			player->_marblesInHome[i] = false;
			break;
		}
	}
	
	dassert(IsValidMarbleColor(mc));
	return mc;
}

void PlayerRestoreMarble(TGMPlayer* player, MarbleColor mc)
{
	int i = MarbleColorToOffset(mc);
	dassert(!player->_marblesInHome[i]);
	player->_marblesInHome[i] = true;
}

void PlayerGameStarting(TGMPlayer* player, TGMGame* game)
{
	PlayerSetGame(player, game);
	for (int i = 0; i < kMarblesPerPlayer; i++)
	{
		player->_marblesInHome[i] = true;
	}
	
	PlayerEventOnGameStarting(player);
}

void PlayerGameEnding(TGMPlayer* player, TGMGame* game)
{
	PlayerEventOnGameEnding(player);
	dassert(game == player->_game);
	(void)game;
	PlayerSetGame(player, NULL);
}

void PlayerStartingHand(TGMPlayer* player)
{
	PlayerEventOnHandStarting(player);
	if (player->_hand) ReleaseCardList(player->_hand);
	player->_hand = CreateCardList();
}

void PlayerDealtCard(TGMPlayer* player, TGMCard* card)
{
	CardListAdd(player->_hand, card);
	PlayerEventOnDealtCard(player, card);
}

void PlayerSetOnGameStarting(TGMPlayer* player, TGMPlayerGameStartingFunction onGameStarting)
{
	player->_onGameStarting = onGameStarting;
}

void PlayerSetOnGameEnding(TGMPlayer* player, TGMPlayerGameEndingFunction onGameEnding)
{
	player->_onGameEnding = onGameEnding;
}

void PlayerSetOnHandStarting(TGMPlayer* player, TGMPlayerHandStartingFunction onHandStarting)
{
	player->_onHandStarting = onHandStarting;
}

void PlayerSetOnDealtCard(TGMPlayer* player, TGMPlayerDealtCardFunction onDealtCard)
{
	player->_onDealtCard = onDealtCard;
}

void PlayerSetOnPlayInGame(TGMPlayer* player, TGMPlayerPlayInGameFunction onPlayInGame)
{
	player->_onPlayInGame = onPlayInGame;
}

void PlayerSetOnDescription(TGMPlayer* player, TGMPlayerDescriptionFunction onDescription)
{
	player->_onDescription = onDescription;
}

void PlayerSetOnDestroy(TGMPlayer* player, TGMPlayerDestroyFunction onDestroy)
{
	player->_onDestroy = onDestroy;
}

void PlayerEventOnGameStarting(TGMPlayer* player)
{
	if (player->_onGameStarting != NULL)
	{
		player->_onGameStarting(player);
	}
}

void PlayerEventOnGameEnding(TGMPlayer* player)
{
	if (player->_onGameEnding != NULL)
	{
		player->_onGameEnding(player);
	}
}

void PlayerEventOnHandStarting(TGMPlayer* player)
{
	if (player->_onHandStarting != NULL)
	{
		player->_onHandStarting(player);
	}
}

void PlayerEventOnDealtCard(TGMPlayer* player, TGMCard* card)
{
	if (player->_onDealtCard != NULL)
	{
		player->_onDealtCard(player, card);
	}
}

void PlayerPlayInGame(TGMPlayer *player)
{
	if (player->_onPlayInGame != NULL)
	{
		player->_onPlayInGame(player);
	}
}
