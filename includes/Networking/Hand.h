#pragma once

#include "PlayingCards/Card.hpp"
#include <vector>
#include <ostream>

namespace Networking
{
	struct Hand
	{
		std::vector<PlayingCards::Card> cards;
		
		friend std::ostream& operator << (std::ostream& o, const Hand& target);
	};
}
