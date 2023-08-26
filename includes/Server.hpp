#ifndef SERVER_HPP
# define SERVER_HPP

#include <netinet/in.h> //in_addr_t type
#include <iostream> 
#include <string>
#include <map>
#include "Location.hpp"

class Server
{
    public:
        /*OCCF*/
        Server( void );
        ~Server( void );
        Server( std::string serverBlock );
        Server( Server& src );
        Server& operator=( const Server& src );

    /*attributes*/
    /*methods*/
    //getters
    //setters
    private:
        /*attributes*/
        unsigned int                _port;
        in_addr_t                   _host;
        std::string                 _serverName;
        bool                        _autoIndex;
        std::map<int, std::string>  _errorPages;
        std::vector<Location>       _locations;
        int                         _listenFd;

};

#endif