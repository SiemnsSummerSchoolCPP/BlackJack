#pragma once

#include <string>

namespace Networking
{
	class Client
	{
	public:
		struct ClientException;
		struct DisconnectedException;
	
		Client();
		
		void setup(const char* ip, int port);
		std::string read() const;
		void send(std::string msg) const;
		
	private:
		static const int maxMsgLen = 1024;
		int m_socket = -1;
	};
	
	/*
	** Exceptions.
	*/
	
	struct Client::ClientException : public std::exception
	{
	private:
		const std::string m_msg;
		
	public:
		ClientException(const std::string msg = "");
		const char* what() const throw();
	};
	
	struct Client::DisconnectedException : public ClientException
	{
		const char* what() const throw();
	};
}
