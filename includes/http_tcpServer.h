#ifndef HTTP_TCPSERVER
#define HTTP_TCPSERVER

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

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
    
    private:
    // Functions
        int     startServer( void );
        void    closeServer( void );   
    
    // Member variable
        std::string         _sIpAddress;
        int                 _serverPort;
        int                 _serverSocket;
        int                 _serverNewSocket;
        long                _serverIncomingMessage;
    
    // Server address
        struct sockaddr_in  _serverAddress;
        unsigned int        _socketAddressLen;
        std::string         _serverMessage;
    // serverAddress.sin_family = AF_INET;
    // serverAddress.sin_port = htons(8080); // Port 8080
    // serverAddress.sin_addr.s_addr = INADDR_ANY;
};

void log(const std::string &message){
    std::cerr << message << std::endl;
}
void exitWithError(const std::string &errorMessage){
    log("ERROR: " + errorMessage);
    exit(1);
}

#endif