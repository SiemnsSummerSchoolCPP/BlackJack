#include "Hand.h"

using namespace Networking;

std::ostream& Networking::operator << (std::ostream& o, const Hand& target)
{
	for (const auto& card : target.cards)
	{
		o << card << " ";
	}
	return o;
}
