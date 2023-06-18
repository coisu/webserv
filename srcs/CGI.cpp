#include "CGI.hpp"

CGI::CGI(Request& request)// : _env(construct_env(request))
{
	_env = construct_env(request);
	std::cout << "CGI created\n";
}

CGI::~CGI()
{
	std::cout << "CGI destroyed\n";
}

CGI::CGI(const CGI& copy)
{
	std::cout << "CGI is being copied\n";
	*this = copy;
}

CGI&	CGI::operator = (const CGI& copy)
{
	std::cout << "CGI is being assigned\n";
	if (this != &copy)
	{
		// this->_body = copy.getBody().copy()
	}
	return (*this);
}

std::map<std::string, std::string>	CGI::construct_env(Request& request)
{
	(void)request;
	this->_env["SERVER_SOFTWARE"] = "Jisu, Yoel and Amanda's Software ;)";
	this->_env["SERVER_NAME"] = "127.0.0.1"; // config.getName();
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["SERVER_PROTOCOL"] = "HTML/1.1";
	this->_env["SERVER_PORT"] = "8081"; // config.getHostPort();
	this->_env["REQUEST_METHOD"] = request.getMethodStr();
	this->_env["PATH_INFO"] = "banana";//extractPathInfo(request.getURL());
	this->_env["PATH_TRANSLATED"] = "banana";
	this->_env["SCRIPT_NAME"] = "banana";
	this->_env["QUERY_STRING"] = "banana";
	this->_env["REMOTE_HOST"] = "banana";
	this->_env["REMOTE_ADDR"] = "banana";
	this->_env["AUTH_TYPE"] = "banana";
	this->_env["REMOTE_IDENT"] = "banana";
	this->_env["CONTENT_TYPE"] = "banana";
	this->_env["CONTENT_LENGTH"] = "banana";
	this->_env["HTTP_ACCEPT"] = "banana";
	this->_env["HTTP_ACCEPT_LANGUAGE"] = "banana";
	this->_env["HTTP_USER_AGENT"] = "banana";
	this->_env["HTTP_COOKIE"] = "banana";
	return (this->_env);
}

char**	CGI::getCharEnv( void )
{
	char**	char_env = new char*[this->_env.size() + 1];
	int		i = 0;

	for (std::map<std::string, std::string>::iterator it = this->_env.begin(); it != this->_env.end(); it++, i++)
	{
		std::string elem = it->first + "=" + it->second;
		char_env[i] = new char[elem.size() + 1];
		strcpy(char_env[i], elem.c_str());
	}
	char_env[i] = NULL;
	// std::cout << "\n\n-------ENV-------\n" << std::endl;
	// i = 0;
	// for (std::map<std::string, std::string>::iterator it = this->_env.begin(); it != this->_env.end(); it++)
	// {
	// 	std::cout << char_env[i] << std::endl;
	// 	i++;
	// }
	// std::cout << "\n\n-----------------\n" << std::endl;
	return (char_env);
}