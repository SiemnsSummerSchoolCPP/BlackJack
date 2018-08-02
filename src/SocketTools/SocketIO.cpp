#include "SocketIO.hpp"
#include <cstdint>
#include <unistd.h>

using namespace SocketTools;

unsigned char* SocketIO::getReceiveBuf() const
{
	return m_receiveBuf;
}

size_t SocketIO::getReceiveBufLen() const
{
	return m_receiveBufLen;
}

int SocketIO::sendData(
	const int socket,
	const void* const data,
	const MsgLenType dataLen)
{
	if (write(socket, &dataLen, sizeof(dataLen)) == -1)
		return -1;
	
	if (write(socket, data, dataLen) == -1)
		return -1;
	
	return 0;
}

int SocketIO::receiveData(const int socket)
{
	MsgLenType msgLen;
	int readValue;
	
	readValue = read(socket, &msgLen, sizeof(msgLen));
	if (readValue == 0)
		return 0;
	
	if (readValue == -1)
		return -1;
	
	if (msgLen > m_receiveBufLen)
	{
		if (m_receiveBuf)
		{
			delete[] m_receiveBuf;
		}
		
		m_receiveBuf = new unsigned char[msgLen + 1];
		m_receiveBufLen = msgLen;
	}
	
	readValue = read(socket, m_receiveBuf, msgLen);
	m_receiveBuf[msgLen] = 0;
	
	if (msgLen == 0)
		return 1;
	else
		return readValue;
}
