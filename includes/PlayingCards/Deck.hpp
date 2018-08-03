#pragma once

#include "Card.hpp"
#include <array>
#include <random>

namespace PlayingCards
{
	class Deck
	{
	public:
		struct NotEnoughCards;
	
		Deck();
		~Deck();
		
		void shuffle();
		Card& dealCard();

	private:
		static const int nbOfCards = 52;
		
		Card** m_cards = nullptr;
		int m_cardIndex;
		
		static Card** freshDeck();
	};
	
	struct Deck::NotEnoughCards : public std::exception
	{
		const char* what() const throw();
	};
}
