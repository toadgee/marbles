use crate::CardSuit;
use crate::CardNumber;

pub struct Card {
	// next: Card,
	// previous: Card,
	pub unique_id: i32,
	pub suit: CardSuit,
	pub number: CardNumber
}