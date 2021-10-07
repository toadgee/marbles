//
//  DataExtensions.cpp
//  Marbles
//
//  Created by Todd on 1/8/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#include "precomp.h"
#include "DebugAssert.h"
#include "MarblesMem.h"
#include "DataExtensions.h"

static const uint8_t kCardDataVersion = 1;
static const uint8_t kDeckDataVersion = 1;
static const uint8_t kGameLogDataVersion = 1;
static const uint8_t kMarbleDataVersion = 1;
static const uint8_t kMoveDataVersion = 1;

static const uint8_t kCardDataMarker = 0;
static const uint8_t kDeckDataMarker = 1;
static const uint8_t kGameLogDataMarker = 2;
static const uint8_t kMarbleDataMarker = 3;
static const uint8_t kMoveDataMarker = 4;

static const uint8_t kMarblesMajorVersion = 1;
static const uint8_t kMarblesMinorVersion = 0;
static const uint8_t kMarblesBuildVersion = 0;

TGMData::TGMData(int initialSize) noexcept
{
	m_data.reserve(static_cast<size_t>(initialSize));
}

void TGMData::WriteData(TGMData&& data) noexcept
{
	m_data.insert(m_data.cend(), data.m_data.begin(), data.m_data.end());
}

void TGMData::WriteHeader(uint8_t marker, uint8_t version) noexcept
{
	m_data.push_back(marker);
	m_data.push_back(version);
}

void TGMData::WriteBool(bool value) noexcept
{
	m_data.push_back(value ? 1 : 0);
}

bool TGMData::ReadBool(TGMDataIterator& iterator) noexcept
{
	uint8_t value = *iterator; ++iterator;
	return (value == 1);
}

void TGMData::WriteMarbleColor(MarbleColor mc) noexcept
{
	dassert(static_cast<int>(mc) < 256);
	m_data.push_back(static_cast<uint8_t>(mc));
}

MarbleColor TGMData::ReadMarbleColor(TGMDataIterator& iterator) noexcept
{
	uint8_t value = *iterator++;
	return static_cast<MarbleColor>(value);
}

void TGMData::WriteInt16(uint16_t value) noexcept
{
	m_data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
	m_data.push_back(static_cast<uint8_t>(value & 0xFF));
}

int16_t TGMData::ReadInt16(TGMDataIterator& iterator) noexcept
{
	int16_t value = *iterator++;
	value <<= 8; value += *iterator++;
	return value;
}

void TGMData::WriteInt(int value) noexcept
{
	m_data.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
	m_data.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
	m_data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
	m_data.push_back(static_cast<uint8_t>(value & 0xFF));
}

int TGMData::ReadInt(TGMDataIterator& iterator) noexcept
{
	int value = *iterator++;
	value <<= 8; value += *iterator++;
	value <<= 8; value += *iterator++;
	value <<= 8; value += *iterator++;
	return value;
}

void TGMData::WriteCardNumber(CardNumber number) noexcept
{
	WriteInt16(static_cast<uint16_t>(number));
}

CardNumber TGMData::ReadCardNumber(TGMDataIterator& iterator) noexcept
{
	uint16_t value = *iterator; ++iterator;
	value <<= 8; value += *iterator++;
	return static_cast<CardNumber>(value);
}

void TGMData::WriteCardSuit(CardSuit suit) noexcept
{
	WriteInt16(static_cast<uint16_t>(suit));
}

CardSuit TGMData::ReadCardSuit(TGMDataIterator& iterator) noexcept
{
	uint16_t value = *iterator++;
	value <<= 8; value += *iterator++;
	return static_cast<CardSuit>(value);
}

void TGMData::WritePlayerColor(PlayerColor pc) noexcept
{
	dassert(static_cast<int>(pc) < 256);
	m_data.push_back(static_cast<uint8_t>(pc));
}

PlayerColor TGMData::ReadPlayerColor(TGMDataIterator& iterator) noexcept
{
	uint8_t value = *iterator++;
	return static_cast<PlayerColor>(value);
}

void TGMData::WriteUInt8(uint8_t value) noexcept
{
	m_data.push_back(value);
}

uint8_t TGMData::ReadUInt8(TGMDataIterator& iterator) noexcept
{
	return *iterator++;
}

void TGMData::WriteStrategy(Strategy strategy) noexcept
{
	WriteInt16(static_cast<uint16_t>(strategy));
}

Strategy TGMData::ReadStrategy(TGMDataIterator& iterator) noexcept
{
	uint16_t value = *iterator++;
	value <<= 8; value += *iterator++;
	return static_cast<Strategy>(value);
}

TGMDataIterator TGMData::StartReading() const noexcept
{
	return m_data.cbegin();
}

bool TGMData::IsAtEnd(const TGMDataIterator& iterator) const noexcept
{
	return m_data.cend() == iterator;
}

bool TGMData::ReadFromFile(const std::string& filename) noexcept
{
	std::fstream f;
	f.open(filename, std::ios::in);
	if (!f.is_open())
	{
		return false;
	}

	std::get(f);

}

uint16_t MarbleDataLength() noexcept
{
	return (
		sizeof(uint8_t) // marker
		 + sizeof(uint8_t) // version
		 + sizeof(uint8_t) // null marble
		 + sizeof(uint8_t) // color
		 + sizeof(int16_t) // distanceFromHome
		 + sizeof(uint8_t)); // wentBehindHome
}

uint16_t CardDataLength() noexcept
{
	return (
		sizeof(uint8_t)  // marker
		+ sizeof(uint8_t) // version
		+ sizeof(int) // unique Id
		+ sizeof(uint16_t) // number
		+ sizeof(uint16_t)); // suit
}

TGMCard* CreateCardFromData(TGMDataIterator& iter) noexcept
{
	uint8_t marker = TGMData::ReadUInt8(iter);
	if (marker != kCardDataMarker)
	{
		dassert(false);
		return nullptr;
	}
	
	uint8_t version = TGMData::ReadUInt8(iter);
	if (version != kCardDataVersion)
	{
		dassert(false);
		return nullptr;
	}
	
	int uniqueId = TGMData::ReadInt(iter);
	CardNumber cn = TGMData::ReadCardNumber(iter);
	CardSuit cs = TGMData::ReadCardSuit(iter);
	return CreateCard(uniqueId, cn, cs);
}

TGMData GetCardData(TGMCard *card) noexcept
{
	TGMData data(CardDataLength());
	data.WriteHeader(kCardDataMarker, kCardDataVersion);
	data.WriteInt(card->_uniqueId);
	data.WriteCardNumber(card->_number);
	data.WriteCardSuit(card->_suit);
	return data;
}

TGMData GetDeckData(TGMDeck *deck) noexcept
{
	TGMData data;
	data.WriteHeader(kDeckDataMarker, kDeckDataVersion);

	int i = 0;
	TGMCard* card = deck->_cards->_first;
	data.WriteInt(static_cast<int>(CardListCount(deck->_cards)));
	while (card != nullptr)
	{
		data.WriteInt(i++);
		data.WriteData(GetCardData(card));
		card = card->nextCard;
	}
	
	i = 0;
	card = deck->_discarded->_first;
	data.WriteInt(static_cast<int>(CardListCount(deck->_discarded)));
	while (card != nullptr)
	{
		data.WriteInt(i++);
		data.WriteData(GetCardData(card));
		card = card->nextCard;
	}
	
	return data;
}

TGMDeck* CreateDeckFromData(TGMDataIterator &iter) noexcept
{
	uint8_t marker = TGMData::ReadUInt8(iter);
	if (marker != kDeckDataMarker)
	{
		dassert(false);
		return nullptr;
	}
	
	uint8_t version = TGMData::ReadUInt8(iter);
	if (version != kDeckDataVersion)
	{
		dassert(false);
		return nullptr;
	}
	
	TGMDeck* deck = CreateDeck(true);
	
	int cardCount = TGMData::ReadInt(iter);
	for (int i = 0; i < cardCount; i++)
	{
		int j = TGMData::ReadInt(iter);
		if (i != j)
		{
			dassert(false);
			ReleaseDeck(deck);
			return nullptr;
		}
		
		TGMCard* card = CreateCardFromData(iter);
		if (card == nullptr)
		{
			dassert(false);
			ReleaseDeck(deck);
			return nullptr;
		}
		
		CardListTransfer(deck->_cards, card);
	}
	
	int discardCount = TGMData::ReadInt(iter);
	for (int i = 0; i < discardCount; i++)
	{
		int j = TGMData::ReadInt(iter);
		if (i != j)
		{
			dassert(false);
			ReleaseDeck(deck);
			return nullptr;
		}
		
		TGMCard* card = CreateCardFromData(iter);
		if (card == nullptr)
		{
			dassert(false);
			ReleaseDeck(deck);
			return nullptr;
		}
		
		CardListTransfer(deck->_discarded, card);
	}
	
	return deck;
}

TGMGameLog* CreateGameLogFromData(TGMData &data) noexcept
{
	// gets the major, minor, build versions
	TGMDataIterator iter{ data.StartReading() };
	if (data.IsAtEnd(iter)) {
		return nullptr;
	}

	int v1 = TGMData::ReadUInt8(iter);
	int v2 = TGMData::ReadUInt8(iter);
	int v3 = TGMData::ReadUInt8(iter);
	if (v1 != kMarblesMajorVersion || v2 != kMarblesMinorVersion || v3 != kMarblesBuildVersion)
	{
		return nullptr;
	}
	
	int marker = TGMData::ReadUInt8(iter);
	if (marker != kGameLogDataMarker)
	{
		dassert(false);
		return nullptr;
	}
	
	int version = TGMData::ReadUInt8(iter);
	if (version != kGameLogDataVersion)
	{
		dassert(false);
		return nullptr;
	}
	
	TGMGameLog* gameLog = CreateGameLog();
	PlayerColor pc = TGMData::ReadPlayerColor(iter);
	GameLogSetDealingPlayer(gameLog, pc);
	
	for (unsigned p = 0; p < kPlayers; p++)
	{
		Strategy strategy = TGMData::ReadStrategy(iter);
		if (!IsValidStrategy(strategy))
		{
			dassert(false);
			return nullptr;
		}
		
		gameLog->_playerStrategy[p] = strategy;
	}
	
	uint16_t deckCount = static_cast<uint16_t>(TGMData::ReadInt(iter));
	for (uint16_t d = 0; d < deckCount; d++)
	{
		uint16_t dEncoded = static_cast<uint16_t>(TGMData::ReadInt(iter));
		if (d != dEncoded)
		{
			dassert(false);
			return nullptr;
		}
		
		TGMDeck* deck = CreateDeckFromData(iter);
		if (!deck)
		{
			dassert(false);
			return nullptr;
		}
		
		DeckListAdd(gameLog->_deckList, deck);
		ReleaseDeck(deck);
	}
	
	int moveCount = TGMData::ReadInt(iter);
	for (int i = 0; i < moveCount; i++)
	{
		int iEnc = TGMData::ReadInt(iter);
		if (i != iEnc)
		{
			dassert(false);
			return nullptr;
		}
		
		TGMMove* move = CreateMoveFromData(iter);
		if (move == nullptr)
		{
			dassert(false);
			return nullptr;
		}
		
		dassert(gameLog != nullptr);
		if (gameLog != nullptr)
		{
			MoveListTransfer(gameLog->_moveList, move);
		}
		else
		{
			ReleaseMove(move);
		}
	}
	
	return gameLog;
}

TGMData GameLogData(TGMGameLog* gameLog) noexcept
{
	// not perfect as it's not the serialized move size, but close enough
	TGMData data(3 + static_cast<int>((sizeof(int) + MoveListCount(gameLog->_moveList) * sizeof(TGMMove))));
	
	// version
	data.WriteUInt8(kMarblesMajorVersion);
	data.WriteUInt8(kMarblesMinorVersion);
	data.WriteUInt8(kMarblesBuildVersion);
	
	data.WriteHeader(kGameLogDataMarker, kGameLogDataVersion);
	data.WritePlayerColor(gameLog->_dealingPlayer);
	
	for (unsigned p = 0; p < kPlayers; p++)
	{
		data.WriteStrategy(gameLog->_playerStrategy[p]);
	}
	
	data.WriteInt(static_cast<int>(DeckListCount(gameLog->_deckList)));
	TGMDeck* deck = gameLog->_deckList->_first;
	int d = 0;
	while (deck != nullptr)
	{
		data.WriteInt(d++);
		data.WriteData(GetDeckData(deck));
		deck = deck->_nextDeck;
	}
	
	data.WriteInt(static_cast<int>(MoveListCount(gameLog->_moveList)));
	TGMMove *move = gameLog->_moveList->first;
	int i = 0;
	while (move != nullptr)
	{
		data.WriteInt(i++);
		data.WriteData(GetMoveData(move));
		move = move->nextMove;
	}
	
	return data;
}

TGMMarble* CreateMarbleFromData(TGMDataIterator& iter, bool* nullMarbleEncoded) noexcept
{
	uint8_t marker = TGMData::ReadUInt8(iter);
	if (marker != kMarbleDataMarker)
	{
		dassert(false);
		return nullptr;
	}
	
	uint8_t version = TGMData::ReadUInt8(iter);
	if (version != kMarbleDataVersion)
	{
		dassert(false);
		return nullptr;
	}
	
	bool nullMarble = TGMData::ReadBool(iter);
	MarbleColor mc = TGMData::ReadMarbleColor(iter);
	int16_t distanceFromHome = TGMData::ReadInt16(iter);
	bool wentBehindHome = TGMData::ReadBool(iter);
	
	if (nullMarbleEncoded)
	{
		*nullMarbleEncoded = nullMarble;
	}
	
	if (nullMarble)
	{
		return nullptr;
	}
	
	return CreateMarble(mc, distanceFromHome, wentBehindHome);
}

TGMData GetMarbleData(TGMMarble *marble) noexcept
{
	TGMData data(MarbleDataLength());
	data.WriteHeader(kMarbleDataMarker, kMarbleDataVersion);
	
	bool isNullMarble = true;
	MarbleColor mc = MarbleColor::None;
	uint16_t distanceFromHome = 0;
	bool wentBehindHome = false;
	
	if (marble)
	{
		isNullMarble = false;
		mc = marble->color;
		distanceFromHome = static_cast<uint16_t>(marble->distanceFromHome);
		wentBehindHome = marble->wentBehindHome;
	}
	
	data.WriteBool(isNullMarble);
	data.WriteMarbleColor(mc);
	data.WriteInt16(distanceFromHome);
	data.WriteBool(wentBehindHome);
	
	return data;
}

TGMMove* CreateMoveFromData(TGMDataIterator& iter) noexcept
{
	uint8_t marker = TGMData::ReadUInt8(iter);
	if (marker != kMoveDataMarker)
	{
		dassert(false);
		return nullptr;
	}
	
	uint8_t version = TGMData::ReadUInt8(iter);
	if (version != kMoveDataVersion)
	{
		dassert(false);
		return nullptr;
	}

	bool nullMarbleEncoded = false;
	TGMMarble* marble = CreateMarbleFromData(iter, &nullMarbleEncoded);
	if (marble == nullptr && !nullMarbleEncoded)
	{
		dassert(false);
		return nullptr;
	}
	
	TGMCard* card = CreateCardFromData(iter);
	if (card == nullptr)
	{
		dassert(false);
		return nullptr;
	}
	
	PlayerColor playerColor = TGMData::ReadPlayerColor(iter);
	int oldSpot = TGMData::ReadInt(iter);
	int newSpot = TGMData::ReadInt(iter);
	bool isDiscard = TGMData::ReadBool(iter);
	int moves = TGMData::ReadInt(iter);
	bool wentBehindHome = TGMData::ReadBool(iter);
	int jumps = TGMData::ReadInt(iter);
	TGMMove* move = MakeMove(card, marble, isDiscard, playerColor, oldSpot, newSpot, moves, jumps, wentBehindHome);
	if (marble != nullptr) ReleaseMarble(marble);
	ReleaseCard(card);
	return move;
}

inline uint16_t MoveDataLength() noexcept
{
	return sizeof(int) // marker
		+ sizeof(int) // version
		+ MarbleDataLength() // marble
		+ CardDataLength() // card
		+ sizeof(uint16_t) // color
		+ sizeof(int) // old spot
		+ sizeof(int) // new spot
		+ sizeof(bool) // isDiscard
		+ sizeof(int) // moves
		+ sizeof(bool) // went behind home
		+ sizeof(int) ; // jumps
}

TGMData GetMoveData(TGMMove *move) noexcept
{
	TGMData data(MoveDataLength());
	data.WriteHeader(kMoveDataMarker, kMoveDataVersion);
	data.WriteData(GetMarbleData(move->marble));
	data.WriteData(GetCardData(move->card));
	data.WritePlayerColor(move->playerColor);
	data.WriteInt(move->oldSpot);
	data.WriteInt(move->newSpot);
	data.WriteBool(move->isDiscard);
	data.WriteInt(move->moves);
	data.WriteBool(move->wentBehindHome);
	data.WriteInt(move->jumps);
	return data;
}

