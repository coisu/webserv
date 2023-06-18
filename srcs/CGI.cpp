#include "CGI.hpp"

CGI::CGI(Request& request)// : _env(construct_env(request))
{
	_env = construct_env(request);
	std::cout << "CGI created\n";
}

CGI::~CGI()
{
	// std::cout << "CGI destroyed\n";
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
	std::map<std::string, std::string> env;

	env["SERVER_SOFTWARE"] = "Jisu, Yoel and Amanda's Software ;)";
	env["SERVER_NAME"] = temp_config.name;
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SERVER_PROTOCOL"] = "HTML/1.1";
	env["SERVER_PORT"] = SSTR(temp_config.host_port);
	env["REQUEST_METHOD"] = request.getMethodStr();
	env["PATH_INFO"] = extractPathInfo(request.getURL());
	env["PATH_TRANSLATED"] = "banana";
	env["SCRIPT_NAME"] = "banana";
	env["QUERY_STRING"] = "banana";
	env["REMOTE_HOST"] = "banana";
	env["REMOTE_ADDR"] = "banana";
	env["AUTH_TYPE"] = "banana";
	env["REMOTE_IDENT"] = "banana";
	env["CONTENT_TYPE"] = "banana";
	env["CONTENT_LENGTH"] = "banana";
	env["HTTP_ACCEPT"] = "banana";
	env["HTTP_ACCEPT_LANGUAGE"] = "banana";
	env["HTTP_USER_AGENT"] = "banana";
	env["HTTP_COOKIE"] = "banana";
	return (env);
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
	std::cout << "\n\n-------ENV-------\n" << std::endl;
	i = 0;
	for (std::map<std::string, std::string>::iterator it = this->_env.begin(); it != this->_env.end(); it++)
	{
		std::cout << char_env[i] << std::endl;
		i++;
	}
	std::cout << "\n\n-----------------\n" << std::endl;
	return (char_env);
}
