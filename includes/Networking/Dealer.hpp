#pragma once

#include "Hand.h"
#include "PlayingCards/Deck.hpp"

namespace Networking
{
	class Dealer
	{
	public:
		Dealer();
	
		const Hand& getHand() const;
		Hand& getHand();
		
		const PlayingCards::Card& dealACard();
		void dealCardsToAHand(Hand& hand, const size_t count = 2);
		
	private:
		Hand m_hand;
		PlayingCards::Deck m_deck;
	};
}
