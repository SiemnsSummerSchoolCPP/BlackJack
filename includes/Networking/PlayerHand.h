#pragma once

#include "Hand.h"
#include "Bet.h"
#include <ostream>

namespace Networking
{
	struct PlayerHand : public Hand
	{
		Bet bet;
		bool standing = false;
		
		friend std::ostream& operator << (std::ostream& o, const PlayerHand& h);
	};
}
