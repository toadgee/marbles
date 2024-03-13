pub enum CardSuit {
	Hearts,
	Diamonds,
	Clubs,
	Spades,
	Joker,
}

impl CardSuit {
	pub fn value(&self) -> u16 {
		return match *self {
			CardSuit::Hearts => 800,
			CardSuit::Diamonds => 801,
			CardSuit::Clubs => 802,
			CardSuit::Spades => 803,
			CardSuit::Joker => 804,
		}
	}
}
