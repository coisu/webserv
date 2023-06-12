#include "http_tcpServer.h"

class Method
{
	public:
		Method();
		~Method();
		Method(const Method& copy);
		Method(std::string idea);
		Method&	operator = (const Method& copy);
	private:
		std::string	_body;
		int			_type;
};