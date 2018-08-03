#pragma once

#include "Hand.h"
#include "Bet.h"
#include <ostream>

namespace Networking
{
	struct PlayerHand : public Hand
	{
		Bet bet;
		
		friend std::ostream& operator << (std::ostream& o, const PlayerHand& h);
	};
}
