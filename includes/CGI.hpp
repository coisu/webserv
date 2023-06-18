#ifndef CGI_HPP
# define CGI_HPP
#include <map>
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

class CGI
{
	public:
		//OCCF
		CGI(Request& request);
		virtual ~CGI();
		CGI(const CGI& copy);
		CGI&	operator = (const CGI& copy);
		
		//public methods
		char**	getCharEnv( void );

		//public attributes
		// char const**	cgi_env;

	protected:
		//private methods

		//private attributes
		std::map<std::string, std::string>	construct_env( Request& request );
		std::string	_query;
		std::map<std::string, std::string>	_env;
};

#endif