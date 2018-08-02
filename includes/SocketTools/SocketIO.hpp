#pragma once

#include <cstddef>
#include <cstdint>

namespace SocketTools
{
	class SocketIO
	{
	public:
		using MsgLenType = uint32_t;
		
		unsigned char* getReceiveBuf() const;
		size_t getReceiveBufLen() const;
		
		int sendData(int socket, const void* data, MsgLenType dataLen);
		int receiveData(int socket);
	
	private:
		unsigned char* m_receiveBuf = nullptr;
		size_t m_receiveBufLen = 0;
	};
}
