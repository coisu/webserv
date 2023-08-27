#include "Server.hpp"

Server::Server( void ) {}

Server::~Server( void ) {}

Server::Server(std::string serverBlock, std::vector<Location> locationVec)
{
    (void)serverBlock;
    this->_locations = locationVec;
    this->_block = serverBlock;
    std::cout << "server block: " << serverBlock << std::endl;
    for (size_t i = 0; i < this->_locations.size(); i++)
    {
        std::cout << "location " << i << ":\n" << this->_locations[i].getBlock() << std::endl;
    }
    std::cout << "\npush server\n";
    // std::cout << "SERVER:\n" << *this << std::endl;
}

Server::Server( const Server& src )
{
    this->_port = src._port;
    this->_host = src._host;
    this->_serverName = src._serverName;
    this->_autoIndex = src._autoIndex;
    this->_errorPages = src._errorPages;
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
        this->_autoIndex = src._autoIndex;
        this->_errorPages = src._errorPages;
        this->_locations = src._locations;
        this->_listenFd = src._listenFd;
        this->_block = src._block;
    }
	return (*this);

}

std::ostream& operator<<(std::ostream& os, const Server& server)
{
    // os << "Server Info:" << std::endl;
    os << "port " << server._port << std::endl;
    os << "host " << server._host << std::endl;
    os << "serverName " << server._serverName << std::endl;
    os << "autoIndex " << server._autoIndex << std::endl;
    os << "errorPages: [";
    for (std::map<int, std::string>::const_iterator it = server._errorPages.begin(); it != server._errorPages.end(); it++)
        os << it->first << " ";
    os << "]";
    for (size_t i = 0; i < server._locations.size(); i++)
        os << "locations " << server._locations[i] << std::endl;
    os << "listenFd " << server._listenFd << std::endl;
    os << "block " << server._block << std::endl;    
    return os;
}

void    Server::setLocations( std::vector<Location> locationVec )
{
    this->_locations = locationVec;
}