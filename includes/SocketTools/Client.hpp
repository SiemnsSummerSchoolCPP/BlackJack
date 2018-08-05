#pragma once

#include "SocketIO.hpp"

namespace SocketTools
{
	class Client
	{
	public:
		// Getters.
		SocketIO& getSocketIOTools();
		int getSocket() const;
		
		void setup(const char* ip, int port);
		
	private:
		int m_socket = -1;
		SocketIO m_socketIOTools;
	};
}
