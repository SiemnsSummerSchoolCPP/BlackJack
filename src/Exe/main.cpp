#include "PlayingCards/Card.hpp"

#include <iostream>

using namespace PlayingCards;

int main()
{
	auto card = Card(Card::Suit::kHearts, Card::Rank::kAce);
	std::cout << card << std::endl;
	return 0;
}
