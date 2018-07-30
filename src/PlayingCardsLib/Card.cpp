#include "Card.hpp"

using namespace PlayingCards;

/*
** Public Getters
*/

Card::Suit Card::getSuit() const { return m_suit; }
Card::Rank Card::getRank() const { return m_rank; }

/*
** Cosntructors & destructors.
*/

Card::Card(const Card::Suit suit, const Card::Rank rank) :
	m_suit(suit),
	m_rank(rank)
{
}

/*
** Private methods.
*/

const char* Card::suitStr() const
{
	switch (m_suit)
	{
		case Card::Suit::kSpades:
			return "♠";
		case Card::Suit::kClubs:
			return "♣";
		case Card::Suit::kDiamonds:
			return "♦";
		case Card::Suit::kHearts:
			return "♥";
	}
}

const char* Card::rankStr() const
{
	switch (m_rank)
	{
		case Card::Rank::kTwo:
			return "2";
		case Card::Rank::kThree:
			return "3";
		case Card::Rank::kFour:
			return "4";
		case Card::Rank::kFive:
			return "5";
		case Card::Rank::kSix:
			return "6";
		case Card::Rank::kSeven:
			return "7";
		case Card::Rank::kEight:
			return "8";
		case Card::Rank::kNine:
			return "9";
		case Card::Rank::kTen:
			return "10";
		case Card::Rank::kJack:
			return "J";
		case Card::Rank::kQueen:
			return "Q";
		case Card::Rank::kKing:
			return "K";
		case Card::Rank::kAce:
			return "A";
	}
}

/*
** Operators.
*/

std::ostream& PlayingCards::operator << (std::ostream& o, const Card& target)
{
	return o << target.suitStr() << target.rankStr();
}

/*
** Limits.
*/

Card::Rank std::numeric_limits<Card::Rank>::min()
{
	return Card::Rank::kTwo;
}

Card::Rank std::numeric_limits<Card::Rank>::max()
{
	return Card::Rank::kAce;
}
