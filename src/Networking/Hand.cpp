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
	
	if (GameManager::isBlackjack(target))
		o << "Blackjack";
	else
	{
		if (GameManager::isBusted(points))
			o << "BUSTED: ";
		o << points << "p";
	}
	o << "]";
	return o;
}
