#include "Hand.h"
#include "GameManager.hpp"

using namespace Networking;

std::ostream& Networking::operator << (std::ostream& o, const Hand& target)
{
	for (const auto& card : target.cards)
	{
		o << card << " ";
	}
	const auto points = GameManager::computeHandPoints(target);
	o << "[";
	
	if (points == GameManager::blackjackPoints)
		o << "Blackjack";
	else if (points > GameManager::blackjackPoints)
		o << "BUSTED";
	else
		o << points << " points";
	
	o << "]";
	return o;
}
