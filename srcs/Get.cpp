#include "Get.hpp"

Get::Get(std::string request) : Method(request)
{
	std::cout << "Get created\n";
}

Get::~Get()
{
	std::cout << "Get destroyed\n";
}

Get::Get(const Get& copy) : Method(copy._head)
{
	std::cout << "Get is being copied\n";
	*this = copy;
}

Get&	Get::operator = (const Get& copy)
{
	std::cout << "Get is being assigned\n";
	if (this != &copy)
	{
		// this->_body = copy.getBody().copy()
	}
	return (*this);
}