#pragma once

#include "PlayerHand.h"
#include <string>
#include <vector>
#include <ostream>

namespace Networking
{
	class Player
	{
	public:
		Player(double& money);
		
		double getMoney() const;
		const std::vector<PlayerHand>& getHands() const;
		std::vector<PlayerHand>& getHands();
		
		void setMoney(double newValue);
		
		std::ostream& printHands(std::ostream& o) const;
		
	private:
		double& m_money;
		std::vector<PlayerHand> m_hands;
	};
}
