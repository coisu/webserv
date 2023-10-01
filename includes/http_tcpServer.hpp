#ifndef HTTP_TCPSERVER
#define HTTP_TCPSERVER

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sstream>
#include <stdlib.h>
#include <fcntl.h>
#include "utils.h"

// struct sockaddr_in {
//     short            sin_family;   // e.g. AF_INET
//     unsigned short   sin_port;     // e.g. htons(8080)
//     struct in_addr   sin_addr;     // see struct in_addr, below
//     //char             sin_zero[8];  // zero this if you want to
// };

// struct in_addr {
//     unsigned long s_addr;
// };

#define BUFFER_SIZE 1024

// struct Tconfig
// {
// 	const std::string	root = "/workspaces/webserv";
// };

class TcpServer
{
    public:

    // Constructor & Copy constructor
        TcpServer( std::string ip_adress, int port );
        TcpServer( const TcpServer& copy );
    // Destructor
        ~TcpServer();
    // Assignement Operator
        TcpServer& operator=( const TcpServer& src );
        
        void    runServer( void );

    private:
    // Methods
        void    acceptConnection( void );
        int     startServer( void );
        void    closeServer( void ); 
        void    startListen( void );
    
    // Member variable
        std::string         _sIpAddress;
        int                 _serverPort; // = _port
        int                 _serverSocket; // = _server_fd
        int					_clientSocket; //this vector will store  socket clients which reached server 
        long                _serverIncomingMessage;
        fd_set              _socketSet;
        int                 _maxSocket;
        // char				_buffer[BUFFER_SIZE + 1];
        // bool                _closeConnexion;

        //private:
		//int						_listen_fd;
		//bool					_close_connexion;
		//bool					_compress_array;// any time a clients disconnected  this variable will be set to true to squeeze poll array
		//bool					_hasError;
    
    // Server address
        struct sockaddr_in  _serverSocketAddress;
        unsigned int        _socketAddressLen;
        std::string         _serverMessage;
    
    // Timeout
        struct timeval      _timeout;
};

// Here we need to see how we want to do the error management


#endif