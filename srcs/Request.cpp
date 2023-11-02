#include "Request.hpp"
Request::Request(Server& serv) : _server(serv){}

Request::Request(std::map<std::string, std::string> header, std::string body, std::string info, Server& serv) 
: _server(serv), _info(info), _head(header), _body(body) 
{

	std::string key, val;
	std::string methods[4] = {"GET", "POST", "DELETE", "INVALID"};

	this->_method_enum = extractMethodType(this->_info);
	this->_method_str = methods[this->_method_enum];
	this->_url = extractURL(this->_info);
	this->_locPath = this->_server.getRoot() + this->_url.substr(0, this->_url.find_first_of('?'));
	ft_logger("CONSTRUCT locPath: " + this->_locPath, DEBUG, __FILE__, __LINE__);
	ft_logger("INFO: " + this->_info, DEBUG, __FILE__, __LINE__);
}

Request::~Request() {} 

Request::Request(const Request& copy) : _server(copy._server)
{
	ft_logger("Request is being copied", DEBUG, __FILE__, __LINE__);
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
	ft_logger("Request is being assigned", DEBUG, __FILE__, __LINE__);
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

	std::stringstream ss;
	ss << "\n\n-------FULL-------\n" 
		<< this->_full_request 
		<< "\n--------END---------\n"
		<< "\n\n-------HEAD-------\n";
	while (it != this->_head.end())
	{
		ss << "[" << it->first << " : " << it->second << "]" << std::endl;
		++it;
	}
	ss << "\n\n-------BODY-------\n" << std::endl;
	ss << this->_body << std::endl;
	ss << "\n--------END---------\n" << std::endl;
	ft_logger(ss.str(), DEBUG, __FILE__, __LINE__);
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
	if (i >= 3)
	{
		ft_logger("INVALID METHOD: " + type, ERROR, __FILE__, __LINE__);
		throw 400;
	}
	return ((t_method)i);
}

std::string	Request::extractURL(std::string info)
{
	size_t	i = 0, n = 0;

	if (info.find("http://") != std::string::npos)
		i = info.find("http://") + 7;
	i = info.find_first_of('/', i);
	if (i == std::string::npos)
		return (std::string());
	while (i + n < info.size() && !std::isspace(info[i + n]))
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
