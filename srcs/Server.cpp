#include "Server.hpp"

Server::Server( void ) {}

Server::~Server( void ) {}

Server::Server(std::string serverBlock)
{
    std::cout << "server block: " << serverBlock << std::endl;
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