#include "Request.hpp"

Request::Request(std::string request, Server& serv, std::map<std::string, std::string> head) 
: server(serv), _full_request(request), _head(head)
{
	parseRequest(request);
	// std::cout << "Request created\n";
	// this->_body = readFile(this->_locPath);
}

Request::Request(std::map<std::string, std::string> header, std::string body, Server& serv) 
: server(serv), _head(header)
{
	parseRequest(request);
	// std::cout << "Request created\n";
	// this->_body = readFile(this->_locPath);
}

Request::~Request()
{
	// if (this->_cgi)
		// delete this->_cgi;
	// std::cout << "Request destroyed\n";
}

Request::Request(const Request& copy) : server(copy.server)//, _cgi(copy.getCGI())
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

// Location*	Request::extractLocation(const Server& server, std::string locPath)
// {
// 	std::vector<Location> locations = server.getLocations();

// 	for (std::vector<Location>::iterator it = locations.begin(); 
// 	it != locations.end(); it++)
// 	{
// 		if ((*it).getPath() == locPath)
// 			this->_location = new Location(*it);
// 	}
// 	return (NULL);
// }

void	Request::parseRequest(std::string request)
{
	// std::map<std::string, std::string> requestHeader;
	std::string key, val;
	std::istringstream iss(request);
	std::string methods[3] = {"GET", "POST", "DELETE"};

	// std::getline(iss, this->_info);

	this->_method_enum = extractMethodType(this->_info);
	this->_method_str = methods[this->_method_enum];
	this->_url = extractURL(this->_info);
	// std::vector<std::string> urlvec = splitUrl(this->_url);
	std::cout << "root: " << this->server.getRoot() << std::endl;
	std::cout << "url: " << this->_url << std::endl;
	std::cout << "suby: " << this->_url.substr(0, this->_url.find_first_of('?')) << std::endl;
	this->_locPath = this->server.getRoot() + this->_url.substr(0, this->_url.find_first_of('?'));
	// this->_location = extractLocation(this->server, this->_locPath);
	this->_isDir = pathIsDir(this->_locPath);
	// this->_is_cgi = (this->_url.find(temp_config.cgi_folder) == 0);
	// this->_is_cgi = this->_location->getIsCGI();

	// while(std::getline(std::getline(iss, key, ':') >> std::ws, val))
		// requestHeader[key] = val.substr(0, val.size() - 1);

	// return requestHeader;
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
	// _body.append("THIS IS 413 TESTING");
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

// CGI*	Request::getCGI() const
// {
// 	return (this->_cgi);
// }

// bool	Request::isCGI() const
// {
// 	return (this->_is_cgi);
// }

bool	Request::UrlIsDir() const
{
	return (this->_isDir);
}

std::string	Request::getBody() const
{
	return (this->_body);
}

t_method	Request::getMethodEnum() const
{
	return (this->_method_enum);
}

std::string	Request::getMethodStr() const
{
	return (this->_method_str);
}

std::string	Request::getInfo() const
{
	return (this->_info);
}

std::string	Request::getURL() const
{
	return (this->_url);
}

std::string Request::getLocPath() const
{
	return (this->_locPath);
}
// std::string	Request::getLocation() const
//{
// 	return (this->_location);
// }

// std::string	Request::getQuery() const
//{
// 	return (this->_query);
// }

std::map<std::string, std::string>	Request::getHead() const
{
	return (this->_head);
}

// Location*	Request::getLocation() const
// {
// 	return (this->_location);_is_dir
// }