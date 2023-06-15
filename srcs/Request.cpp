#include "Request.hpp"

Request::Request(std::string request) : _head(parse_request(request))
{
	std::cout << "Request created\n";
}

Request::Request(std::map<std::string, std::string>	head) : _head(head)
{
	std::cout << "Request created\n";
}

Request::~Request()
{
	std::cout << "Request destroyed\n";
}

Request::Request(const Request& copy)
{
	std::cout << "Request is being copied\n";
	*this = copy;
}

Request&	Request::operator = (const Request& copy)
{
	std::cout << "Request is being assigned\n";
	if (this != &copy)
	{
		// this->_body = copy.getBody().copy()
	}
	return (*this);
}

std::map<std::string, std::string>	Request::parse_request(std::string request)
{
	std::map<std::string, std::string> m;
	std::string key, val;
	std::istringstream iss(request);

	std::getline(iss, this->_info);
	this->_type = getMethodType(this->_info);
	this->_url = getURL(this->_info);
	std::cout << "url: " << this->_url << std::endl
			  << "type: " << this->_type << std::endl;
	while(std::getline(std::getline(iss, key, ':') >> std::ws, val))
		m[key] = val.substr(0, val.size() - 2);
	return m;
}

void	Request::printHead( void )
{
	std::map<std::string, std::string>::iterator it = this->_head.begin();

	std::cout << "\n\n-------HEAD-------\n" << std::endl;
	while (it != this->_head.end())
	{
		std::cout << "[" << it->first << " : " << it->second << "]" << std::endl;
		++it;
	}
	std::cout << "\n--------END---------\n" << std::endl;
}
