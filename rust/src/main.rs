/*use crate::game::card::Card;
use crate::game::cardSuit::CardSuit;
use crate::game::cardNumber::CardNumber;

pub mod game;
*/

fn main() {
	let c = Card { unique_id: 3, suit: CardSuit::Hearts, number: CardNumber::Ace };
	println!("Hello, world! {} {} {}", c.unique_id, c.suit.value(), c.number.value());
}
