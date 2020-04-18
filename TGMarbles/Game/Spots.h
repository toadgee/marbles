#pragma once

#define kMaxMoveSpots          14 // the maximum amount of spots a marble can move
#define kMoveSpotUnused       255 // unused marker in the spots for the move
#define kFinalSpotsOffset (2 * kTotalSpots) // make it 2 times bigger so it doesn't look like we forgot to wrap
#define kPlayerSpots		   18 // number of spots per player
#define kGetOutSpot			   -1 // position we mark get outs at
#define kTotalSpots (kPlayerSpots * kPlayers)
#define kGetInSpot (kPlayerSpots * kPlayers - kPlayerStartingPosition)
#define IsValidSpot(x) ((x) >= 0 && (x) < kTotalSpots)
#define IsValidFinalSpot(x) ((x) >= 0 && (x) < kMarblesPerPlayer)
#define IsValidHomeSpot(x) ((x) >= 0 && (x) < kMarblesPerPlayer)
#define kPlayerStartingPosition 4 // starting position for the player
#define kPlayerPointPosition	9 // point offset from player's starting position
#define PositionForPlayerColor(pc) ((pc) - Player_Min)
#define MarbleColorToOffset(mc) (static_cast<int>(mc) % kMarblesPerPlayer)
#define IsFinalSpot(x) ((x) >= kFinalSpotsOffset && (x) < (kFinalSpotsOffset + kMarblesPerPlayer))
#define FinalSpotToSpot(x) ((x) + kFinalSpotsOffset)
#define SpotToFinalSpot(x) ((x) % kTotalSpots)
#define IsPointSpot(spot) (((spot) - kPlayerPointPosition) % kPlayerSpots == 0)
#define IsPlayerStartingSpot(spot) (((spot) - kPlayerStartingPosition) % kPlayerSpots == 0)
#define IsPlayerLastSpot(spot) ((spot) % kPlayerSpots == 0)
#define WrapSpot(spot) (((spot) + kTotalSpots) % kTotalSpots)
#define PlayerColorForLastSpot(spot) ((PlayerColor)(((spot) / kPlayerSpots) + Player_Min))
#define PlayerColorForPosition(position) static_cast<PlayerColor>(Player_Min + (position))
#define PositionForMarble(mc) ((static_cast<int>(mc) - (static_cast<int>(MarbleColor::Min) + 1)) % kMarblesPerPlayer) // 1 to include non-none
#define PlayerSectionStart(pc) (PositionForPlayerColor(pc) * kPlayerSpots)
// this converts the spot to be from 0-total spots from the player's perspective
// adding kTotalSpots helps if it's negative
#define NormalizeSpot(pc, spot) WrapSpot((spot) - PlayerSectionStart(pc) + kTotalSpots)
#define PlayerStartingSpot(pc) PlayerSectionStart(pc) + kPlayerStartingPosition
#define PlayerPointSpot(pc) PlayerSectionStart(pc) + kPlayerPointPosition
#define IsLastPlayerPointSpot(spot, pc) (NormalizeSpot(pc, spot) == (kTotalSpots - kPlayerSpots + kPlayerPointPosition))


