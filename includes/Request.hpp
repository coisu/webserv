#ifndef REQUEST_HPP
# define REQUEST_HPP
#include <map>
#include <iostream>
#include <sstream>
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
		void	printHead( void );

		//public attributes
		char const**	cgi_env;

	protected:
		//private methods
		std::map<std::string, std::string>	parse_request(std::string request);
		std::map<std::string, std::string>	construct_env( struct sockaddr& _serverSocketAddress )


		//private attributes
		std::string	_body;
		int			_type;
		std::string	_info;
		std::string	_url;
		std::string	_location;
		std::string	_query;
		std::map<std::string, std::string>	_head;
		std::map<std::string, std::string>	_cgi_env;
};

#endif