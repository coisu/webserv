#include "Server.hpp"

Server::Server( void ) {}

Server::~Server( void ) {}

Server::Server(std::string serverBlock)
{
    (void)serverBlock;
}

Server::Server( Server& src )
{
    *this = src;

}

Server& Server::operator=( const Server& src )
{
    return (src);

}