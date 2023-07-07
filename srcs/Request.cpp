#include "Request.hpp"

Request::Request(std::string request) : _full_request(request), _head(parseRequest(request))//, _body(extractBody)
{
	// std::cout << "Request created\n";
	// printRequest();
	this->_cgi = NULL;
	if (this->_is_cgi)
		this->_cgi = new CGI(*this);
	else
		_body = readFile(this->_location);
}

// Request::Request(std::map<std::string, std::string>	head) : _head(head)
// {
// 	// std::cout << "Request created\n";
// }

Request::~Request()
{
	if (this->_cgi)
		delete this->_cgi;
	// std::cout << "Request destroyed\n";
}

Request::Request(const Request& copy) : _cgi(copy.getCGI())
{
	std::cout << "Request is being copied\n";
	*this = copy;
}

Request&	Request::operator = (const Request& copy)
{
	std::cout << "Request is being assigned\n";
	if (this != &copy)
	{
		// this->_body = copy.getBody().copy()
	}
	return (*this);
}

// std::string	Request::extractBody(std::string request)
// {
	
// }

std::map<std::string, std::string>	Request::parseRequest(std::string request)
{
	std::map<std::string, std::string> m;
	std::string key, val;
	std::istringstream iss(request);
	std::string methods[3] = {"GET", "POST", "DELETE"};

	std::getline(iss, this->_info);

	this->_method_enum = extractMethodType(this->_info);
	this->_method_str = methods[this->_method_enum];
	this->_url = extractURL(this->_info);
	// std::vector<std::string> urlvec = splitUrl(this->_url);
	this->_location = temp_config.root + this->_url.substr(0, this->_url.find_first_of('?'));
	this->_is_dir = pathIsDir(this->_location);
	this->_is_cgi = (this->_url.find(temp_config.cgi_folder) == 0);

	while(std::getline(std::getline(iss, key, ':') >> std::ws, val))
		m[key] = val.substr(0, val.size() - 1);

	return m;
}

void	Request::printRequest( void )
{
	std::map<std::string, std::string>::iterator it = this->_head.begin();

	std::cout << "\n\n-------FULL-------\n" << std::endl;
	std::cout << this->_full_request << std::endl;
	std::cout << "\n--------END---------\n" << std::endl;
	std::cout << "\n\n-------HEAD-------\n" << std::endl;
	while (it != this->_head.end())
	{
		std::cout << "[" << it->first << " : " << it->second << "]" << std::endl;
		++it;
	}
	std::cout << "\n--------END---------\n" << std::endl;
	std::cout << "\n\n-------BODY-------\n" << std::endl;
	std::cout << this->_body << std::endl;
	std::cout << "\n--------END---------\n" << std::endl;
}

e_method	Request::extractMethodType(std::string info)
{
	size_t		i = 0, n = 0;
	std::string methods[3] = {"GET", "POST", "DELETE"};
	std::string	type;

	while (!std::isspace(info[n]))
		n++;
	type = info.substr(0, n);
	while (!methods[i].empty() && methods[i] != type)
		i++;
	return ((t_method)i);
}

std::string	Request::extractURL(std::string info)
{
	size_t	i, n = 0;

	i = info.find_first_of('/');
	while (!std::isspace(info[i + n]))
		n++;
	return (info.substr(i, n));
}

// std::string	Request::extractUrlNoQuery(std::string url)
// {
// 	return (url.substr(0, url.find_first_of('?')));
// }

//GETTERS

CGI*	Request::getCGI() const{
	return (this->_cgi);
}

bool	Request::isCGI(){
	return (this->_is_cgi);
}

bool	Request::UrlIsDir(){
	return (this->_is_dir);
}

std::string	Request::getBody(){
	return (this->_body);
}

t_method	Request::getMethodEnum(){
	return (this->_method_enum);
}

std::string	Request::getMethodStr(){
	return (this->_method_str);
}

std::string	Request::getInfo(){
	return (this->_info);
}

std::string	Request::getURL(){
	return (this->_url);
}

// std::string	Request::getLocation(){
// 	return (this->_location);
// }

// std::string	Request::getQuery(){
// 	return (this->_query);
// }

std::map<std::string, std::string>	Request::getHead(){
	return (this->_head);
}

