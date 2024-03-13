use crate::game::card::Card;
use crate::game::card_suit::CardSuit;
use crate::game::card_number::CardNumber;

pub mod game;

fn main() {
	let c = Card { unique_id: 3, suit: CardSuit::Hearts, number: CardNumber::Ace };
	println!("Hello, world! {} {} {}", c.unique_id, c.suit.value(), c.number.value());
}
