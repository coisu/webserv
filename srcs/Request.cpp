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

std::map<std::string, std::string>	Request::construct_env( struct sockaddr& _serverSocketAddress )
{
	std::map<std::string, std::string> cgi_env;

	cgi_env["SERVER_SOFTWARE"] =
	cgi_env["SERVER_NAME"] =
	cgi_env["GATEWAY_INTERFACE"] =
	cgi_env["SERVER_PROTOCOL"] =
	cgi_env["SERVER_PORT"] =
	cgi_env["REQUEST_METHOD"] =
	cgi_env["PATH_INFO"] =
	cgi_env["PATH_TRANSLATED"] =
	cgi_env["SCRIPT_NAME"] =
	cgi_env["QUERY_STRING"] =
	cgi_env["REMOTE_HOST"] =
	cgi_env["REMOTE_ADDR"] =
	cgi_env["AUTH_TYPE"] =
	cgi_env["REMOTE_IDENT"] =
	cgi_env["CONTENT_TYPE"] =
	cgi_env["CONTENT_LENGTH"] =
	cgi_env["HTTP_ACCEPT"] =
	cgi_env["HTTP_ACCEPT_LANGUAGE"] =
	cgi_env["HTTP_USER_AGENT"] =
	cgi_env["HTTP_COOKIE"] =
	return (cgi_env);
}

std::map<std::string, std::string>	Request::parse_request(std::string request)
{
	std::map<std::string, std::string> m;
	std::string key, val;
	std::istringstream iss(request);

	std::getline(iss, this->_info);
	this->_type = getMethodType(this->_info);
	this->_url = getURL(this->_info);
	// std::cout << "info: " << this->_info << std::endl;
	std::cout << "url: " << this->_url << std::endl
			  << "type: " << this->_type << std::endl;
	while(std::getline(std::getline(iss, key, ':') >> std::ws, val))
		m[key] = val;
	return m;
}

void	Request::printHead( void )
{
	std::map<std::string, std::string>::iterator it = this->_head.begin();
	while (it != this->_head.end())
	{
		// std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
		std::cout << "[" << it->first << " : " << it->second << "]" << std::endl;
		++it;
	}
}