#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

#include <string>

class Exception
{
public:
	Exception(const std::string &error) : m_errorMsg(error) {}
	~Exception(){}
std::string m_errorMsg;
};

#endif //_EXCEPTION_HPP_
