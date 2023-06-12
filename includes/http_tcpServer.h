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


// struct sockaddr_in {
//     short            sin_family;   // e.g. AF_INET
//     unsigned short   sin_port;     // e.g. htons(8080)
//     struct in_addr   sin_addr;     // see struct in_addr, below
//     //char             sin_zero[8];  // zero this if you want to
// };

// struct in_addr {
//     unsigned long s_addr;
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
        
        void    acceptConnection( void );
    
    private:
    // Methods
        int     startServer( void );
        void    closeServer( void ); 
        void    startListen( void );
    
    // Member variable
        std::string         _sIpAddress;
        int                 _serverPort;
        int                 _serverSocket;
        int                 _clientSocket;
        long                _serverIncomingMessage;
    
    // Server address
        struct sockaddr_in  _serverSocketAddress;
        unsigned int        _socketAddressLen;
        std::string         _serverMessage;
};

// Here we need to see how we want to do the error management


#endif