#include "Server.hpp"

Server::Server( void ) {}

Server::~Server( void ) {}

Server::Server(std::string serverBlock, std::vector<Location> locationVec)
{
    std::stringstream   ss(serverBlock);
    std::string         part;

    this->_locations = locationVec;
    this->_block = serverBlock;
    // std::cout << "server block: " << serverBlock << std::endl;
    // for (size_t i = 0; i < this->_locations.size(); i++)
    // {
    //     std::cout << "location " << i << ":\n" << this->_locations[i].getBlock() << std::endl;
    // }
    // std::cout << "\npush server\n";
    while (std::getline(ss, part, ';'))
    {
        std::string key = part.substr(0, part.find(':'));
        std::string value = part.substr(part.find(':')+1);
        // std::cout << "key: \"" << key << "\" value: \"" << value << "\"\n";
        setAttributes(key, value);
    }
    // std::cout << "SERVER:\n" << *this << std::endl;
}

void    Server::setAttributes(std::string key, std::string value)
{
    const int N = 8;
    std::string keys[N] = {"port", 
                          "host", 
                          "server_name", 
                          "error_page", 
                          "client_body_size", 
                          "root", 
                          "index", 
                          "autoindex"};
    size_t i = 0;
    while (i < N && keys[i] != key)
        i++;
    switch (i)
    {
    case 0:
        initPort(value);
        break;
    case 1:
        initHost(value);
        break;
    case 2:
        initServerName(value);
        break;
    case 3:
        initErrorPage(value);
        break;
    case 4:
        initClientBodySize(value);
        break;
    case 5:
        initRoot(value);
        break;
    case 6:
        initIndex(value);
        break;
    case 7:
        initAutoIndex(value);
        break;
    default:
        throw std::runtime_error("unrecognised server key: " + key);
    }
}

Server::Server( const Server& src )
{
    this->_port = src._port;
    this->_host = src._host;
    this->_serverName = src._serverName;
    this->_errorPages = src._errorPages;
    this->_clientBodySize = src._clientBodySize;
    this->_root = src._root;
    this->_index = src._index;
    this->_autoIndex = src._autoIndex;
    this->_locations = src._locations;
    this->_listenFd = src._listenFd;
    this->_block = src._block;

}

Server& Server::operator=( const Server& src )
{
    if (this != &src)
	{
        this->_port = src._port;
        this->_host = src._host;
        this->_serverName = src._serverName;
        this->_errorPages = src._errorPages;
        this->_clientBodySize = src._clientBodySize;
        this->_root = src._root;
        this->_index = src._index;
        this->_autoIndex = src._autoIndex;
        this->_locations = src._locations;
        this->_listenFd = src._listenFd;
        this->_block = src._block;
    }
	return (*this);

}

std::ostream& operator<<(std::ostream& os, const Server& server)
{
    // os << "Server Info:" << std::endl;
    os << "port: " << server._port << std::endl;
    os << "host: " << server._host << std::endl;
    os << "serverName: " << server._serverName << std::endl;
    os << "errorPages: [";
    for (std::map<int, std::string>::const_iterator it = server._errorPages.begin(); it != server._errorPages.end(); it++)
        os << it->first << ", " << it->second << " ";
    os << "]\n";
    os << "client body size: " << server._clientBodySize << std::endl;
    os << "root: " << server._root << std::endl;
    os << "index: " << server._index << std::endl;
    os << "autoIndex: " << server._autoIndex << std::endl;
    os << "------------LOCATIONS-------------\n";
    for (size_t i = 0; i < server._locations.size(); i++)
        os << "location " << i + 1 << ":\n" << server._locations[i] << std::endl;
    os << "----------------------------------\n";
    // os << "listenFd " << server._listenFd << std::endl;
    // os << "block " << server._block << std::endl;    
    return os;
}

void    Server::setLocations( std::vector<Location> locationVec )
{
    this->_locations = locationVec;
}

void Server::initPort(std::string value)
{
    this->_port = htons(atoi(value.c_str()));
}

void Server::initHost(std::string value)
{
    this->_host = inet_addr(value.c_str());
}

void Server::initServerName(std::string value)
{
    this->_serverName = value;
}

void Server::initErrorPage(std::string value)
{
    std::stringstream   ss(value);
    std::string         code;
    std::string         path;
    
    while (std::getline(ss, code, ',') && std::getline(ss, path, ','))
    {
        this->_errorPages[atoi(code.c_str())] = path;
    }
}

void Server::initClientBodySize(std::string value)
{
    this->_clientBodySize = atoi(value.c_str());
}

void Server::initRoot(std::string value)
{
    this->_root = value;
}

void Server::initIndex(std::string value)
{
    this->_index = value;
}

void Server::initAutoIndex(std::string value)
{
    this->_autoIndex = (value == "on") ? true : false;
}

unsigned int	Server::getPort() const
{
	return (this->_port);
}

in_addr_t	Server::getHost() const
{
	return (this->_host);
}

std::string	Server::getServerName() const
{
	return (this->_serverName);
}

std::map<int, std::string>	Server::getErrorPages() const
{
	return (this->_errorPages);
}

size_t	Server::getClientBodySize() const
{
	return (this->_clientBodySize);
}

std::string	Server::getRoot() const
{
	return (this->_root);
}

std::string	Server::getIndex() const
{
	return (this->_index);
}

bool	Server::getAutoIndex() const
{
	return (this->_autoIndex);
}

std::vector<Location>	Server::getLocations() const
{
	return (this->_locations);
}

int	Server::getListenFd() const
{
	return (this->_listenFd);
}

std::string	Server::getBlock() const
{
	return (this->_block);
}
