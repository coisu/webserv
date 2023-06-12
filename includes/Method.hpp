#ifndef METHOD_HPP
# define METHOD_HPP
#include <map>
#include <iostream>
#include <sstream>
#include "utils.h"

class Method
{
	public:
		//OCCF
		Method(std::string request);
		Method(std::map<std::string, std::string>	head);
		virtual ~Method();
		Method(const Method& copy);
		Method&	operator = (const Method& copy);
		
		//public methods
		void	printHead( void );


	protected:
		//private methods
		std::map<std::string, std::string>	parse_request(std::string request);

		//private attributes
		std::string	_body;
		int			_type;
		std::string	_info;
		std::string	_url;
		std::map<std::string, std::string>	_head;
};

#endif