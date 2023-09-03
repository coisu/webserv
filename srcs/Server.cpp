#include "Server.hpp"

Server::Server( void ) {}

Server::~Server( void ) {}

Server::Server(std::string serverBlock, std::vector<Location> locationVec)
{
    std::stringstream   ss(serverBlock);
    std::string         part;

    this->_locations = locationVec;
    this->_block = serverBlock;

    /*Fill Server Values*/
    while (std::getline(ss, part, ';'))
    {
        std::string key = part.substr(0, part.find(':'));
        std::string value = part.substr(part.find(':')+1);
        setAttributes(key, value);
    }

    /*Verify Server and Location values*/
    //verify error pages
    // if ()

}

void    Server::setAttributes(std::string key, std::string value)
{
    size_t      N = 6;
    std::string keys[N] = {"port", 
                          "host", 
                          "server_name", 
                          "error_page", 
                          "client_body_size", 
                          "root"}; 
                        //   "index", 
                        //   "autoindex"};
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
    // case 6:
    //     initIndex(value);
    //     break;
    // case 7:
    //     initAutoIndex(value);
    //     break;
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
    // this->_index = src._index;
    // this->_autoIndex = src._autoIndex;
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
        // this->_index = src._index;
        // this->_autoIndex = src._autoIndex;
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
        os << it->first << ", " << it->second << " | ";
    os << "]\n";
    os << "client body size: " << server._clientBodySize << std::endl;
    os << "root: " << server._root << std::endl;
    // os << "index: " << server._index << std::endl;
    // os << "autoIndex: " << server._autoIndex << std::endl;
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
    if (value.find_first_not_of("0123456789") != value.npos || value.size() > 5)
        throw std::runtime_error("invalid port in server block.");
    int val = atoi(value.c_str());
    if (val < 1 || val > 65535)
        throw std::runtime_error("port must be between 1 and 65,535.");
    this->_port = htons(val);
}

void Server::initHost(std::string value)
{
  	std::string part;
	std::string ip[4];
	std::stringstream ss(value);
    int i = 0;
    if (value.find_first_not_of("0123456789.") != value.npos || value.size() > 15)
        throw std::runtime_error("(1) invalid host address in server block.");
    while (std::getline(ss, part, '.'))
    {
        if (i == 4)
            throw std::runtime_error("(2) invalid host address in server block.");
        ip[i++] = part;
    }
    for (int j = 0; j < 4; j++)
    {
        if (ip[j].size() == 0)
            throw std::runtime_error("(3) invalid host address in server block.");
        if (atoi(ip[j].c_str()) > 255 || atoi(ip[j].c_str()) < 0)
            throw std::runtime_error("(4) invalid host address in server block.");
    }
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
        if (code.size() > 3 || code.find_first_not_of("0123456789") != code.npos)
            throw std::runtime_error("invalid error page code.");
        int stat = atoi(code.c_str());
        if (stat > 599 || stat < 400)
            throw std::runtime_error("error codes must be in the range: 400-599.");
        if (path[0] != '/')
            throw std::runtime_error("error page path must start with \'/\' - path: " + path);
        if (path.at(path.length() - 1) == '/' && path.size() > 1)
            throw std::runtime_error("error page path must *not* end with \'/\' - path: " + path);
        this->_errorPages[stat] = path;
    }
}

void Server::initClientBodySize(std::string value)
{
    if (value.size() > 18 || value.find_first_not_of("0123456789") != value.npos)
        throw std::runtime_error("bad client body size.");
    this->_clientBodySize = static_cast<size_t>(atoll(value.c_str()));
}

void Server::initRoot(std::string value)
{
    if (value[0] != '/')
        throw std::runtime_error("root path must start with \'/\' - path: " + value);
    if (value.at(value.length() - 1) == '/' && value.size() > 1)
        throw std::runtime_error("root path must *not* end with \'/\' - path: " + value);
    if (!pathIsDir(value))
        throw std::runtime_error("root path is not a valid directory - path: " + value);
    // if (!pathExists(value))
    //     throw std::runtime_error("bananaaaaaaaaaaaaa");
    this->_root = value;
}

// void Server::initIndex(std::string value)
// {
//     this->_index = value;
// }

// void Server::initAutoIndex(std::string value)
// {
//     this->_autoIndex = (value == "on") ? true : false;
// }

std::string Server::getRoot() const
{
    return (this->_root);
}