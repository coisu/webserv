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
// #include "http_tcpServer.hpp"
#include "utils.hpp"
#include "Server.hpp"
#include "Request.hpp"

class CGI
{
	public:
		//OCCF
		// CGI(Server& serv, std::string RequestUrl, std::string methodString, std::map<std::string, std::string> cgiConfig);
		CGI(Server& serv, Location& location, Request& request);
		virtual ~CGI( void );
		CGI(const CGI& copy);
		CGI&	operator = (const CGI& copy);
		
		//public methods
		char**		getCharEnv( void );
		std::string	exec_cgi( void );
		// std::string exec_cgi(const char* cgi_path, char* const argv[], char* const envp[])

		//public attributes
		const Server&	server;
		// char const**	cgi_env;

	protected:
		//private methods
		std::map<std::string, std::string>	constructEnv(std::string RequestUrl, std::string methodString);
		// std::vector<std::string>	splitUrl(std::string url);
		void	identifyCGI(std::vector<std::string> urlvec);
		std::string	extractScriptName(std::vector<std::string> urlvec);
		std::string	extractPathInfo(std::vector<std::string> urlvec);
		std::string	extractQueryString(std::vector<std::string> urlvec);

		//private attributes
;		std::string	_query;
		std::map<std::string, std::string>	_env;
		Location&	_location;
		Request&	_request;
		std::map<std::string, std::string>	_cgiConfig;
		std::string	_program;
		std::string	_script;
		std::string	_postfix;
		char *_av[3];

};

#endif