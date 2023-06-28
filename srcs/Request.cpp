#include "Request.hpp"

Request::Request(std::string request) : _head(parseRequest(request))// : _body(extractBody))
{
	// std::cout << "Request created\n";
	this->_cgi = NULL;
	if (this->_is_cgi)
		std::cout << "\n----MAKING CGI----\n", this->_cgi = new CGI(*this);
}

Request::Request(std::map<std::string, std::string>	head) : _head(head)
{
	// std::cout << "Request created\n";
}

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
	this->_is_cgi = (this->_url.find(temp_config.cgi_folder) == 1);
	// std::cout << "\nSIZE OF URL: " << this->_url.size() << " SIZE OF CGI: " << temp_config.cgi_folder.size() << std::endl;
	// std::cout << "URL: " << this->_url << " CGI: "<< temp_config.cgi_folder << std::endl;
	if (this->_is_cgi && this->_url.size() <= temp_config.cgi_folder.size() + 1)
		std::cerr << "\n\n----------ERROR-CGI-FOLDER----------\n\n", this->_is_cgi = false;
	this->_is_dir = (this->_is_cgi) ? false : extractDirStatus(this->_url);

	while(std::getline(std::getline(iss, key, ':') >> std::ws, val))
		m[key] = val.substr(0, val.size() - 2);

	return m;
}

void	Request::printHead( void )
{
	std::map<std::string, std::string>::iterator it = this->_head.begin();

	std::cout << "\n\n-------HEAD-------\n" << std::endl;
	while (it != this->_head.end())
	{
		std::cout << "[" << it->first << " : " << it->second << "]" << std::endl;
		++it;
	}
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

bool	Request::extractDirStatus(std::string url)
{
	struct stat		statbuf;

	return ((stat((temp_config.root + url).c_str(), &statbuf) == 0) ? (S_ISDIR(statbuf.st_mode)) : throw 404);
	// throw 404;
	// return (0);
}

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

