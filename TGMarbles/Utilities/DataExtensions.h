//
//  DataExtensions.h
//  Marbles
//
//  Created by Todd on 1/8/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#include "GameLog.h"
#include "Deck.h"
#include "Card.h"

uint16_t MoveDataLength() noexcept;
uint16_t CardDataLength() noexcept;
uint16_t MarbleDataLength() noexcept;

typedef std::vector<uint8_t>::const_iterator TGMDataIterator;

class TGMData final
{
public:
	TGMData() noexcept {}
	TGMData(int initialSize) noexcept;

	void WriteHeader(uint8_t marker, uint8_t version) noexcept;
	void WriteBool(bool value) noexcept;
	void WriteMarbleColor(MarbleColor mc) noexcept;
	void WriteUInt8(uint8_t value) noexcept;
	void WriteInt16(uint16_t value) noexcept;
	void WriteInt(int value) noexcept;
	void WriteCardNumber(CardNumber number) noexcept;
	void WriteCardSuit(CardSuit suit) noexcept;
	void WritePlayerColor(PlayerColor pc) noexcept;
	void WriteData(TGMData&& data) noexcept;
	void WriteStrategy(Strategy strategy) noexcept;
	
	TGMDataIterator StartReading() const noexcept;
	static bool ReadBool(TGMDataIterator& iterator) noexcept;
	static MarbleColor ReadMarbleColor(TGMDataIterator& iterator) noexcept;
	static uint8_t ReadUInt8(TGMDataIterator& iterator) noexcept;
	static int16_t ReadInt16(TGMDataIterator& iterator) noexcept;
	static int ReadInt(TGMDataIterator &iterator) noexcept;
	static CardNumber ReadCardNumber(TGMDataIterator &iterator) noexcept;
	static CardSuit ReadCardSuit(TGMDataIterator &iterator) noexcept;
	static PlayerColor ReadPlayerColor(TGMDataIterator &iterator) noexcept;
	static Strategy ReadStrategy(TGMDataIterator& iterator) noexcept;

private:
	std::vector<uint8_t> m_data;
};

TGMGameLog* CreateGameLogFromData(TGMData& data) noexcept;
TGMData GameLogData(TGMGameLog* gameLog) noexcept;
TGMCard* CreateCardFromData(TGMDataIterator &iterator) noexcept;
TGMData GetCardData(TGMCard *card) noexcept;
TGMData GetDeckData(TGMDeck *deck) noexcept;
TGMDeck* CreateDeckFromData(TGMDataIterator& iterator) noexcept;
TGMData GetMarbleData(TGMMarble *marble) noexcept;
TGMMove* CreateMoveFromData(TGMDataIterator& iterator) noexcept;
TGMData GetMoveData(TGMMove *move) noexcept;
TGMMarble* CreateMarbleFromData(TGMDataIterator& iterator, bool* nullMarbleEncoded) noexcept;
