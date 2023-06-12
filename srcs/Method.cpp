#include "Method.hpp"

Method::Method(std::string request) : _head(parse_request(request))
{
	std::cout << "Method created\n";
}

Method::Method(std::map<std::string, std::string>	head) : _head(head)
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

std::map<std::string, std::string>	Method::parse_request(std::string request)
{
	std::map<std::string, std::string> m;
	std::string key, val;
	std::istringstream iss(request);
	size_t	i, n = 0;

	std::getline(iss, this->_info);
	i = this->_info.find_first_of('/');
	while (!std::isspace(this->_info[i + n]))
		n++;
	this->_url = this->_info.substr(i, n);
	std::cout << "info: " << this->_info << std::endl;
	std::cout << "url: " << this->_url << std::endl;

	while(std::getline(std::getline(iss, key, ':') >> std::ws, val))
		m[key] = val;
	return m;
}

void	Method::printHead( void )
{
	std::map<std::string, std::string>::iterator it = this->_head.begin();
	while (it != this->_head.end())
	{
		// std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
		std::cout << "[" << it->first << " : " << it->second << "]" << std::endl;
		++it;
	}
}