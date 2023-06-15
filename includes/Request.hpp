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
#include "utils.h"

class Request
{
	public:
		//OCCF
		Request(std::string request);
		Request(std::map<std::string, std::string>	head);
		virtual ~Request();
		Request(const Request& copy);
		Request&	operator = (const Request& copy);
		
		//public methods
		void	printHead();
		std::string	getBody();
		t_method	getMethodEnum();
		std::string	getMethodStr();
		std::string	getInfo();
		std::string	getURL();
		std::string	getLocation();
		std::string	getQuery();
		std::map<std::string, std::string>	getHead();
		bool		UrlIsDirectory();
		
		//public attributes
		Tconfig temp_config;
		// char const**	cgi_env;

	protected:
		//private methods
		std::map<std::string, std::string>	parse_request(std::string request);


		//private attributes
		std::string	_body;
		t_method	_method_enum;
		std::string	_method_str;
		std::string	_info;
		std::string	_url;
		std::string	_location;
		std::string	_query;
		std::map<std::string, std::string>	_head;
};

#endif