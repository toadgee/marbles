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
	pub static fn are_equal(card1: Option<Card>, card2: Option<Card>) -> bool {
		if card1.is_none() && card2.is_none() {
			return true
		}

		if card1.is_none() {
			return false
		}

		if card2.is_none() {
			return false
		}

		let card1Value: Card = card1.expect()
		let card2Value: Card = card2.expect()
	
		return card1Value.number == card2Value.number && card1Value.suit == card2Value.suit && card1Value.unique_id == card2Value.unique_id
	}
/*
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
	}*/
}