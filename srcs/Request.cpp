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
	std::map<std::string, std::string>	cgi_env;
	std::stringstream					ss;

	cgi_env["SERVER_SOFTWARE"] = "banana";
	cgi_env["SERVER_NAME"] = "banana";
	cgi_env["GATEWAY_INTERFACE"] = "banana";
	cgi_env["SERVER_PROTOCOL"] = "banana";
	ss << _serverSocketAddress.sin_port;
	cgi_env["SERVER_PORT"] = ss.str();
	cgi_env["REQUEST_METHOD"] = "banana";
	cgi_env["PATH_INFO"] = "banana";
	cgi_env["PATH_TRANSLATED"] = "banana";
	cgi_env["SCRIPT_NAME"] = "banana";
	cgi_env["QUERY_STRING"] = "banana";
	cgi_env["REMOTE_HOST"] = "banana";
	cgi_env["REMOTE_ADDR"] = "banana";
	cgi_env["AUTH_TYPE"] = "banana";
	cgi_env["REMOTE_IDENT"] = "banana";
	cgi_env["CONTENT_TYPE"] = "banana";
	cgi_env["CONTENT_LENGTH"] = "banana";
	cgi_env["HTTP_ACCEPT"] = "banana";
	cgi_env["HTTP_ACCEPT_LANGUAGE"] = "banana";
	cgi_env["HTTP_USER_AGENT"] = "banana";
	cgi_env["HTTP_COOKIE"] = "banana";
	return (cgi_env);
}

char**	Request::getEnv( void )
{
	// char*	p = {0};
	char**	env = (char **)malloc(sizeof(char *) * this->_cgi_env.size());
	std::map<std::string, std::string>::iterator it = this->_cgi_env.begin();

	for (long unsigned int i = 0; i < this->_cgi_env.size(); i++)
	{
		env[i] = (char *)(it->first + "=" + it->second).c_str();
		it++;
	}
	it = this->_cgi_env.begin();
	std::cout << "env:" << std::endl;
	for (long unsigned int i = 0; i < this->_cgi_env.size(); i++, it++)
		std::cout << env[i] << std::endl;
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