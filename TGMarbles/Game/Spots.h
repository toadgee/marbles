#pragma once

#ifdef __cplusplus
/// The number of spots per player
static constexpr int kPlayerSpots = 18;

/// The number of total spots in the game.
static constexpr int kTotalSpots = (kPlayerSpots * kPlayers);

/// The maximum amount of spots a marble can move
static constexpr int kMaxMoveSpots = 14;

/// Unused marker in the spots for the move.
static constexpr uint8_t kMoveSpotUnused = 255;

/// The offset until we get to the final spots.
///
/// Make it 2 times bigger so it doesn't look like we forgot to wrap
static constexpr int kFinalSpotsOffset = 2 * kTotalSpots;

/// Starting position for the player.
static constexpr int kPlayerStartingPosition = 4;

/// Position we mark get outs at
static constexpr int kGetOutSpot = -1;

/// Position we get in at
static constexpr int kGetInSpot = kPlayerSpots * kPlayers - kPlayerStartingPosition;

/// Point offset from player's starting position
static constexpr int kPlayerPointPosition = 9;

inline bool IsValidSpot(int x) noexcept { return x >= 0 && x < kTotalSpots; }
inline bool IsValidFinalSpot(int x) noexcept { return x >= 0 && x < kMarblesPerPlayer; }
inline bool  IsValidHomeSpot(int x) noexcept { return x >= 0 && x < kMarblesPerPlayer; }

#define PositionForPlayerColor(pc) (static_cast<int>(pc) - static_cast<int>(PlayerColor::Min))
#define MarbleColorToOffset(mc) (static_cast<int>(mc) % kMarblesPerPlayer)

inline bool IsFinalSpot(int x) noexcept { return x >= kFinalSpotsOffset && x < (kFinalSpotsOffset + kMarblesPerPlayer); }
inline int FinalSpotToSpot(int x) noexcept { return x + kFinalSpotsOffset; }
inline int SpotToFinalSpot(int x) noexcept { return x % kTotalSpots; }
#define IsPointSpot(spot) (((spot) - kPlayerPointPosition) % kPlayerSpots == 0)
#define IsPlayerStartingSpot(spot) (((spot) - kPlayerStartingPosition) % kPlayerSpots == 0)
inline bool IsPlayerLastSpot(int spot) noexcept { return spot % kPlayerSpots == 0; }
#define WrapSpot(spot) (((spot) + kTotalSpots) % kTotalSpots)
#define PlayerColorForLastSpot(spot) (static_cast<PlayerColor>(((spot) / kPlayerSpots) + static_cast<int>(PlayerColor::Min)))
#define PlayerColorForPosition(position) static_cast<PlayerColor>(static_cast<int>(PlayerColor::Min) + (position))
#define PositionForMarble(mc) ((static_cast<int>(mc) - (static_cast<int>(MarbleColor::Min) + 1)) % kMarblesPerPlayer) // 1 to include non-none
#define PlayerSectionStart(pc) (PositionForPlayerColor(pc) * kPlayerSpots)
// this converts the spot to be from 0-total spots from the player's perspective
// adding kTotalSpots helps if it's negative
#define NormalizeSpot(pc, spot) WrapSpot((spot) - PlayerSectionStart(pc) + kTotalSpots)
#define PlayerStartingSpot(pc) PlayerSectionStart(pc) + kPlayerStartingPosition
#define PlayerPointSpot(pc) PlayerSectionStart(pc) + kPlayerPointPosition
#define IsLastPlayerPointSpot(spot, pc) (NormalizeSpot(pc, spot) == (kTotalSpots - kPlayerSpots + kPlayerPointPosition))

#endif

