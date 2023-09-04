#ifndef SERVER_HPP
# define SERVER_HPP

#include <netinet/in.h> //in_addr_t type
#include <arpa/inet.h> // inet_addr
#include <iostream> 
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstdlib>
#include "Location.hpp"

class Server
{
    public:
    /*OCCF*/
        Server( void );
        ~Server( void );
        Server( std::string serverBlock, std::vector<Location> locationVec );
        Server( const Server& src );
        Server& operator=( const Server& src );

    /*attributes*/

    /*methods*/
        friend std::ostream& operator<<(std::ostream& os, const Server& server);
        //getters		
        unsigned int                getPort() const;
        in_addr_t                   getHost() const;
        std::string                 getServerName() const;
        std::map<int, std::string>  getErrorPages() const;
        size_t                      getClientBodySize() const;
        std::string                 getRoot() const;
        std::string                 getIndex() const;
        bool                        getAutoIndex() const;
        std::vector<Location>       getLocations() const;
        int                         getListenFd() const;
        std::string                 getBlock() const;
        //setters
        void    setLocations( std::vector<Location> locationVec );
    
	private:
    /*methods*/
        void    setAttributes(std::string key, std::string value);
        void    initPort(std::string value);
        void    initHost(std::string value);
        void    initServerName(std::string value);
        void    initErrorPage(std::string value);
        void    initClientBodySize(std::string value);
        void    initRoot(std::string value);
        void    initIndex(std::string value);
        void    initAutoIndex(std::string value);

    /*attributes*/
        unsigned int                _port;
        in_addr_t                   _host;
        std::string                 _serverName;
        std::map<int, std::string>  _errorPages;
        size_t                      _clientBodySize;
        std::string                 _root;
        std::string                 _index;
        bool                        _autoIndex;
        std::vector<Location>       _locations;
        int                         _listenFd;
        std::string                 _block;

};


#endif