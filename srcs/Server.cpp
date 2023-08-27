#include "Server.hpp"

Server::Server( void ) {}

Server::~Server( void ) {}

Server::Server(std::string serverBlock, std::vector<Location> locationVec)
{
    std::stringstream   ss(serverBlock);
    std::string         part;

    this->_locations = locationVec;
    this->_block = serverBlock;
    std::cout << "server block: " << serverBlock << std::endl;
    for (size_t i = 0; i < this->_locations.size(); i++)
    {
        std::cout << "location " << i << ":\n" << this->_locations[i].getBlock() << std::endl;
    }
    std::cout << "\npush server\n";
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
    size_t      N = 8;
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
    case 1:
        initHost(value);
    case 2:
        initServerName(value);
    case 3:
        initErrorPage(value);
    case 4:
        initClientBodySize(value);
    case 5:
        initRoot(value);
    case 6:
        initIndex(value);
    case 7:
        initAutoIndex(value);
    default:
        throw std::runtime_error("unrecognised server key.");
    }
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
