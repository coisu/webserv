#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include "CGI.hpp"
#include "utils.h"
#include "Server.hpp"


class	CGI;
// class	Server;

class Request
{
	public:
		//OCCF
		Request(std::string request, Server& serv);
		// Request(std::map<std::string, std::string>	head);
		virtual ~Request();
		Request(const Request& copy);
		Request&	operator = (const Request& copy);
		
		//public methods
		void	printRequest();
		//getters
		CGI*		getCGI() const;
		std::string	getBody() const;
		t_method	getMethodEnum() const;
		std::string	getMethodStr() const;
		std::string	getInfo() const;
		std::string	getURL() const;
		std::map<std::string, std::string>	getHead() const;
		Location*	getLocation() const;
		Location*	extractLocation(const Server& server, std::string locPath);
		// std::string	getLocation();
		// std::string	getQuery();
		bool	UrlIsDir() const;
		bool	isCGI() const;
		
		//public attributes
		const Server&	server;
		// char const**	cgi_env;

	protected:
		//private methods
		std::map<std::string, std::string>	parseRequest(std::string request);
		e_method	extractMethodType(std::string info);
		std::string	extractURL(std::string info);
		// bool		extractDirStatus(std::string url);

		//private attributes
		bool		_is_cgi;
		bool		_is_dir;
		t_method	_method_enum;
		std::string	_method_str;
		std::string	_info;
		std::string	_url;
		// std::string	_location;
		Location*	_location;
		CGI*		_cgi;
		std::string	_full_request;
		std::map<std::string, std::string>	_head;
		std::string	_body;
		std::string	_locPath;
		// std::string	_location;
		// std::string	_query;
};

#endif