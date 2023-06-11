#include "http_tcpServer.h"

// Constructor
TcpServer::TcpServer( std::string ipAddress, int port ): _sIpAddress(ipAddress),
_serverPort(port), _serverSocket(), _serverNewSocket(), _serverIncomingMessage(),
_socketAddressLen(sizeof(_socketAddressLen)), _serverMessage(buildResponse()){
    startServer();
}

// Copy Constructor
TcpServer::TcpServer( const TcpServer& copy ){
    *this = copy;
}

// Destructor
TcpServer::~TcpServer(){
    closeServer();
}

// Assignement Operator
TcpServer   &TcpServer::operator=( const TcpServer& src ){
    if (this == &src)
        return *this;
    this->_serverSocket = src._serverSocket;
    return *this;
}

// Functions
int TcpServer::startServer(){
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0)
    {
        exitWithError("Cannot create socket");
        return 1;
    }
    return 0;
}

void TcpServer::closeServer(){
    close(_serverSocket);
    close(_serverNewSocket);
    exit(0);
}