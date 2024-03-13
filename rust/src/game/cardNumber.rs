pub enum CardNumber/*: u16*/ {
	Joker, //       = 700, 
	Ace, //         = 701,
	Card2, //       = 702,
	Card3, //       = 703,
	Card4, //       = 704,
	Card5, //       = 705,
	Card6, //       = 706,
	Card7, //       = 707,
	Card8, //       = 708,
	Card9, //       = 709,
	Card10, //      = 710,
	Jack, //        = 711,
	Queen, //       = 712,
	King, //        = 713,
}

impl CardNumber {
	pub fn value(&self) -> u16 {
		match *self {
			CardNumber::Joker  =>  700, // must be before Ace
			CardNumber::Ace    =>  701,
			CardNumber::Card2  =>  702,
			CardNumber::Card3  =>  703,
			CardNumber::Card4  =>  704,
			CardNumber::Card5  =>  705,
			CardNumber::Card6  =>  706,
			CardNumber::Card7  =>  707,
			CardNumber::Card8  =>  708,
			CardNumber::Card9  =>  709,
			CardNumber::Card10 =>  710,
			CardNumber::Jack   =>  711,
			CardNumber::Queen  =>  712,
			CardNumber::King   =>  713,
		}
	}

	pub fn is_numbered(&self) -> bool {
		return match *self {
			CardNumber::Joker  =>  false,
			CardNumber::Ace    =>  true,
			CardNumber::Card2  =>  true,
			CardNumber::Card3  =>  true,
			CardNumber::Card4  =>  true,
			CardNumber::Card5  =>  true,
			CardNumber::Card6  =>  true,
			CardNumber::Card7  =>  true,
			CardNumber::Card8  =>  true,
			CardNumber::Card9  =>  true,
			CardNumber::Card10 =>  true,
			CardNumber::Jack   =>  true,
			CardNumber::Queen  =>  true,
			CardNumber::King   =>  true,
		}
	}
	
	pub fn moves(&self) -> i32 {
		return match *self {
			CardNumber::Joker  =>  0,
			CardNumber::Ace    =>  1,
			CardNumber::Card2  =>  2,
			CardNumber::Card3  =>  3,
			CardNumber::Card4  => -4,
			CardNumber::Card5  =>  5,
			CardNumber::Card6  =>  6,
			CardNumber::Card7  =>  7,
			CardNumber::Card8  =>  8,
			CardNumber::Card9  =>  9,
			CardNumber::Card10 => 10,
			CardNumber::Jack   => 11,
			CardNumber::Queen  => 12,
			CardNumber::King   => 13,
		}
	}
}

impl fmt::Display for CardNumber {
	fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
		write!(f, "{}", self.value)
	}
}
