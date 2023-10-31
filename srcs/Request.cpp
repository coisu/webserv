#include "Request.hpp"
Request::Request(Server& serv) : _server(serv){}

Request::Request(std::map<std::string, std::string> header, std::string body, std::string info, Server& serv) 
: _server(serv), _info(info), _head(header), _body(body) 
{

	std::string key, val;
	std::string methods[4] = {"GET", "POST", "DELETE", "INVALID"};

	this->_method_enum = extractMethodType(this->_info);
	this->_method_str = this->_method_enum < 3 ? methods[this->_method_enum] : "INVALID";
	this->_url = extractURL(this->_info);
	this->_locPath = this->_server.getRoot() + this->_url.substr(0, this->_url.find_first_of('?'));
	std::cerr << "CONSTRUCT locPath: " << this->_locPath << std::endl;
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
	std::string methods[4] = {"GET", "POST", "DELETE", "INVALID"};
	std::string	type;

	while (n < info.size() && !std::isspace(info[n]))
		n++;
	type = info.substr(0, n);
	while (i < 3 && !methods[i].empty() && methods[i] != type)
		i++;
	return ((t_method)i);
}

std::string	Request::extractURL(std::string info)
{
	size_t	i, n = 0;

	if (info.find("http://") != std::string::npos)
	{
		i = info.find("http://") + 7;
		i = info.find_first_of('/', i);
	}
	if (i == std::string::npos)
		return (std::string());
	while (i + n < info.size() && !std::isspace(info[i + n]))
		n++;
	// check differently for the request which contains http://
	// check for all the enums in the code and make sure theres a case for when it is "INVALID"
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
