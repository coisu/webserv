#include "Method.hpp"

Method::Method()
{
	std::cout << "Method created\n";
}

Method::~Method()
{
	std::cout << "Method destroyed\n";
}

Method::Method(const Method& copy)
{
	std::cout << "Method is being copied\n";
	*this = copy;
}

Method&	Method::operator = (const Method& copy)
{
	std::cout << "Method is being assigned\n";
	if (this != &copy)
	{
		// this->_body = copy.getBody().copy()
	}
	return (*this);
}