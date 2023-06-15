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

std::map<std::string, std::string>	Request::construct_env( struct sockaddr_in& _serverSocketAddress )
{
	// std::map<std::string, std::string>	cgi_env;
	std::stringstream					ss;

	this->_cgi_env["SERVER_SOFTWARE"] = "banana";
	this->_cgi_env["SERVER_NAME"] = "banana";
	this->_cgi_env["GATEWAY_INTERFACE"] = "banana";
	this->_cgi_env["SERVER_PROTOCOL"] = "banana";
	ss << _serverSocketAddress.sin_port;
	this->_cgi_env["SERVER_PORT"] = ss.str();
	this->_cgi_env["REQUEST_METHOD"] = "banana";
	this->_cgi_env["PATH_INFO"] = "banana";
	this->_cgi_env["PATH_TRANSLATED"] = "banana";
	this->_cgi_env["SCRIPT_NAME"] = "banana";
	this->_cgi_env["QUERY_STRING"] = "banana";
	this->_cgi_env["REMOTE_HOST"] = "banana";
	this->_cgi_env["REMOTE_ADDR"] = "banana";
	this->_cgi_env["AUTH_TYPE"] = "banana";
	this->_cgi_env["REMOTE_IDENT"] = "banana";
	this->_cgi_env["CONTENT_TYPE"] = "banana";
	this->_cgi_env["CONTENT_LENGTH"] = "banana";
	this->_cgi_env["HTTP_ACCEPT"] = "banana";
	this->_cgi_env["HTTP_ACCEPT_LANGUAGE"] = "banana";
	this->_cgi_env["HTTP_USER_AGENT"] = "banana";
	this->_cgi_env["HTTP_COOKIE"] = "banana";
	// this->_this->_cgi_env = this->_cgi_env;
	return (this->_cgi_env);
}

char**	Request::getEnv( void )
{
	// char*	p = {0};
	char**	env = new char*[this->_cgi_env.size() + 1];
	int		i = 0;
	// std::map<std::string, std::string>::iterator iti = this->_cgi_env.begin();

	for (std::map<std::string, std::string>::iterator it = this->_cgi_env.begin(); it != this->_cgi_env.end(); it++, i++)
	{
		std::string elem = it->first + "=" + it->second;
		env[i] = new char[elem.size() + 1];
		strcpy(env[i], elem.c_str());
	}
	env[i] = NULL;
	std::cout << "\n\n-------ENV-------\n" << std::endl;
	i = 0;
	for (std::map<std::string, std::string>::iterator it = this->_cgi_env.begin(); it != this->_cgi_env.end(); it++)
	{
		std::cout << env[i] << std::endl;
		i++;
	}
	std::cout << "\n\n-----------------\n" << std::endl;
	return (env);
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
		m[key] = val;
	return m;
}

void	Request::printHead( void )
{
	std::map<std::string, std::string>::iterator it = this->_head.begin();
	while (it != this->_head.end())
	{
		std::cout << "[" << it->first << " : " << it->second << "]" << std::endl;
		++it;
	}
}