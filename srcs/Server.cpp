#include "Server.hpp"

Server::Server( void ) {}

Server::~Server( void ) {}

Server::Server(std::string serverBlock, std::vector<Location> locationVec)
{
    (void)serverBlock;
    this->_locations = locationVec;
    this->_block = serverBlock;
    // std::cout << "server block: " << serverBlock << std::endl;
        std::cout << "SERVER:\n" << serverBlock << std::endl;
    for (size_t i = 0; i < this->_locations.size(); i++)
    {
        std::cout << "location " << i << ":\n" << this->_locations[i].getBlock() << std::endl;
    }
    std::cout << "\npush server\n";
}

Server::Server( const Server& src )
{
    *this = src;

}

Server& Server::operator=( const Server& src )
{
    if (this != &src)
	{}
	return (*this);

}

void    Server::setLocations( std::vector<Location> locationVec )
{
    this->_locations = locationVec;
}