#ifndef GET_HPP
# define GET_HPP
#include "Method.hpp"
#include <map>

class Get : public Method
{
	public:
		//OCCF
		Get(std::string request);
		virtual ~Get();
		Get(const Get& copy);
		Get&	operator = (const Get& copy);
		
		//public methods
		//public attributes

	protected:
		//private methods

		//private attributes
		std::string	_url;
};

#endif