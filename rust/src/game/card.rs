use crate::CardSuit;
use crate::CardNumber;

pub struct Card {
	// next: Card,
	// previous: Card,
	pub unique_id: i32,
	pub suit: CardSuit,
	pub number: CardNumber
}

impl Card {
	bool AreCardsEqual(TGMCard* card1, TGMCard* card2)
	{
		if (card1 == card2) return true;
		if (card1 == nullptr || card2 == nullptr) return false;
	
		return (card1->_number == card2->_number)
			&& (card1->_suit == card2->_suit)
			&& (card1->_uniqueId == card2->_uniqueId);
	}

	bool AreCardsEquivalent(TGMCard* card1, TGMCard* card2)
	{
		if (card1 == nullptr && card2 == nullptr) return true;
		if (card1 == nullptr || card2 == nullptr || (card1->_number != card2->_number))
		{
			return false;
		}
	
		return true;
	}

	TGMCardComparisonResult CompareCards(TGMCard* card1, TGMCard* card2)
	{
		if (card1 == nullptr && card2 == nullptr)
		{
			return TGMCardComparisonResult::Same;
		}
		else if (card1 == nullptr)
		{
			return TGMCardComparisonResult::Ascending;
		}
		else if (card2 == nullptr)
		{
			return TGMCardComparisonResult::Descending;
		}
		else if (card2->_number < card1->_number)
		{
			return TGMCardComparisonResult::Ascending;
		}
		else if (card2->_number > card1->_number)
		{
			return TGMCardComparisonResult::Descending;
		}
		else if (card2->_suit < card1->_suit)
		{
			return TGMCardComparisonResult::Ascending;
		}
		else if (card2->_suit > card1->_suit)
		{
			return TGMCardComparisonResult::Descending;
		}
		else if (card2->_uniqueId < card1->_uniqueId)
		{
			return TGMCardComparisonResult::Ascending;
		}
		else if (card2->_uniqueId > card1->_uniqueId)
		{
			return TGMCardComparisonResult::Descending;
		}
	
		return TGMCardComparisonResult::Same;
	}

	TGMCardComparisonResult CompareCardsForDiscard(TGMCard* card1, TGMCard* card2)
	{
		TGMCardComparisonResult regularCompare = CompareCards(card1, card2);
		if (regularCompare == TGMCardComparisonResult::Ascending) return TGMCardComparisonResult::Descending;
		if (regularCompare == TGMCardComparisonResult::Descending) return TGMCardComparisonResult::Ascending;
		return TGMCardComparisonResult::Same;
	}
}