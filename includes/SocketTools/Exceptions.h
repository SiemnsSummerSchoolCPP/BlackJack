#pragma once

#include <exception>
#include <string>

namespace SocketTools { namespace Exceptions
{
	struct SocketException : public std::exception
	{
		virtual const char* what() const throw() = 0;
	};
	
	struct SocketExceptionStr : public SocketException
	{
	public:
		SocketExceptionStr(std::string msg);
		const char* what() const throw() override;
		
	private:
		const std::string m_msg;
	};
}}
