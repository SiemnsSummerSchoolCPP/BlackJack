#pragma once

#include "Hand.h"
#include "PlayingCards/Deck.hpp"

namespace Networking
{
	class Dealer
	{
	public:
	private:
		Hand m_hand;
		PlayingCards::Deck m_deck;
	};
}
