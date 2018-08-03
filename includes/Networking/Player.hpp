#pragma once

#include "PlayerHand.h"
#include <string>
#include <vector>

namespace Networking
{
	struct Player
	{
	public:
		Player(const std::string& name, double& money);
		
		double getMoney() const;
		const Bet& getBet() const;
		const std::vector<PlayerHand>& getHands() const;
		std::vector<PlayerHand>& getHands();
		
		void setMoney(double newValue);
		
	private:
		const std::string& m_name;
		double& m_money;
		std::vector<PlayerHand> m_hands;
	};
}
