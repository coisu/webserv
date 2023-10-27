#include "Request.hpp"
Request::Request(Server& serv) : _server(serv){}

Request::Request(std::map<std::string, std::string> header, std::string body, std::string info, Server& serv) 
: _server(serv), _info(info), _head(header), _body(body) 
{

	std::string key, val;
	std::string methods[3] = {"GET", "POST", "DELETE"};

	this->_method_enum = extractMethodType(this->_info);
	this->_method_str = methods[this->_method_enum];
	this->_url = extractURL(this->_info);
	this->_locPath = this->_server.getRoot() + this->_url.substr(0, this->_url.find_first_of('?'));
}

Request::~Request() {} 

Request::Request(const Request& copy) : _server(copy._server)
{
	// std::cout << "Request is being copied\n";
	this->_full_request = copy._full_request;
	this->_info = copy._info;
	this->_method_enum = copy._method_enum;
	this->_method_str = copy._method_str;
	this->_url = copy._url;
	this->_locPath = copy._locPath;
	this->_head = copy._head;
	this->_body = copy._body;
}

Request&	Request::operator = (const Request& copy)
{
	// std::cout << "Request is being assigned\n";
	if (this != &copy)
	{
		this->_full_request = copy._full_request;
		this->_info = copy._info;
		this->_method_enum = copy._method_enum;
		this->_method_str = copy._method_str;
		this->_url = copy._url;
		this->_locPath = copy._locPath;
		this->_head = copy._head;
		this->_body = copy._body;
	}
	return (*this);
}

void	Request::printRequest( void )
{
	std::map<std::string, std::string>::iterator it = this->_head.begin();

	std::cout << "\n\n-------FULL-------\n" << std::endl;
	std::cout << this->_full_request << std::endl;
	std::cout << "\n--------END---------\n" << std::endl;
	std::cout << "\n\n-------HEAD-------\n" << std::endl;
	while (it != this->_head.end())
	{
		std::cout << "[" << it->first << " : " << it->second << "]" << std::endl;
		++it;
	}
	std::cout << "\n\n-------BODY-------\n" << std::endl;
	// _body = "name=JISU+CHOI&age=10";
	std::cout << this->_body << std::endl;
	std::cout << "\n--------END---------\n" << std::endl;
}

e_method	Request::extractMethodType(std::string info)
{
	size_t		i = 0, n = 0;
	std::string methods[3] = {"GET", "POST", "DELETE"};
	std::string	type;

	while (!std::isspace(info[n]))
		n++;
	type = info.substr(0, n);
	while (!methods[i].empty() && methods[i] != type)
		i++;
	return ((t_method)i);
}

std::string	Request::extractURL(std::string info)
{
	size_t	i, n = 0;

	i = info.find_first_of('/');
	while (!std::isspace(info[i + n]))
		n++;
	return (info.substr(i, n));
}

std::string	Request::getBody() const
{
	return (this->_body);
}

t_method	Request::getMethodEnum() const
{
	return (this->_method_enum);
}

std::string	Request::getMethodStr() const
{
	return (this->_method_str);
}

std::string	Request::getInfo() const
{
	return (this->_info);
}

std::string	Request::getURL() const
{
	return (this->_url);
}

std::string Request::getLocPath() const
{
	return (this->_locPath);
}

std::map<std::string, std::string>	Request::getHead() const
{
	return (this->_head);
}
