#pragma once

#include "SocketIO.hpp"

namespace SocketTools
{
	class Client : public SocketIO
	{
	public:
		// Getters.
		int getSocket() const;
		
		void setup(const char* ip, int port);
		
	private:
		int m_socket = -1;
	};
}
