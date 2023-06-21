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
#include <vector>


class TcpServer
{
    public:

    // Constructor & Copy constructor
        TcpServer( void );
        TcpServer( std::string ip_adress, int port );
        TcpServer( const TcpServer& copy );

    // Destructor
        ~TcpServer();

    // Assignement Operator
        TcpServer& operator=( const TcpServer& src );

    // Public Methods   
        void                runServer( std::vector<int> fdSel );
        void                acceptConnection();
    
    // Public Getters
        int                 getPort( void );
        int                 getServerSocketFd( void );
        std::vector<int>    &getClientSockets( void );

    // Setters
        void                setPort( int port );
        void                setUpServer( void );

    private:

    // Methods
        int                 startServer( void );
        void                closeServer( void );
        void                startListen( void );
    
    // Member variable
        std::string         _sIpAddress;
        int                 _port; 
        int                 _serverSocket; // = _server_fd
        std::vector<int>    _clientSocket; //this vector will store  socket clients which reached server 
        long                _serverIncomingMessage;
        fd_set              _socketSet;
        int                 _maxSocket;
        char				_buffer[BUFFER_SIZE + 1];
        bool                _closeConnexion;

        //private:
		//int						_listen_fd;
		//bool					_close_connexion;
		//bool					_compress_array;// any time a clients disconnected  this variable will be set to true to squeeze poll array
		//bool					_hasError;
    
    // Server address
        struct sockaddr_in  _address;
        unsigned int        _addressLen;
        std::string         _serverMessage;
    
    // Timeout
        struct timeval      _timeout;
};

// Here we need to see how we want to do the error management


#endif