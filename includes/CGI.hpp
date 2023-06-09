#ifndef CGI_HPP
# define CGI_HPP
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include "Request.hpp"
#include "http_tcpServer.h"
#include "utils.h"

class	Request;

class CGI
{
	public:
		//OCCF
		CGI(Request& request);
		virtual ~CGI( void );
		CGI(const CGI& copy);
		CGI&	operator = (const CGI& copy);
		
		//public methods
		char**		getCharEnv( void );
		std::string	exec_cgi( void );

		//public attributes
		// char const**	cgi_env;

	protected:
		//private methods
		std::map<std::string, std::string>	constructEnv(Request& request);
		// std::vector<std::string>	splitUrl(std::string url);
		void	identifyCGI(std::vector<std::string> urlvec);
		std::string	extractScriptName(std::vector<std::string> urlvec);
		std::string	extractPathInfo(std::vector<std::string> urlvec);
		std::string	extractQueryString(std::vector<std::string> urlvec);

		//private attributes
;		std::string	_query;
		std::map<std::string, std::string>	_env;
		std::string	_program;
		std::string	_script;
		std::string	_postfix;
		char *_av[3];
};

#endif