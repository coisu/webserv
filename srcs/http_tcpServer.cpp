#include "../includes/http_tcpServer.h"

// Constructor
TcpServer::TcpServer( std::string ipAddress, int port ): _sIpAddress(ipAddress),
_serverPort(port), _serverSocket(), _clientSocket(), _serverIncomingMessage(),
_socketAddressLen(sizeof(_socketAddressLen)), _serverMessage("hello"){
    startServer();
    _serverSocketAddress.sin_family = AF_INET; // for IPv4
    _serverAddress.sin_port = htons(8080); // call htons to ensure that the port is stored in network byte order
    _serverSocketAddress.sin_addr.s_addr = INADDR_ANY; // is the address 0.0.0.0
    inet_addr(_sIpAddress.c_str()); // convert the IP address from a char * to a unsigned long and have it stored in network byte order
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
    this->_sIpAddress = src._sIpAddress;
    this->_serverPort = src._serverPort;
    this->_serverSocket = src._serverSocket;
    this->_clientSocket = src._clientSocket;
    this->_serverIncomingMessage = src._serverIncomingMessage;
    this->_socketAddressLen = src._socketAddressLen;
    this->_serverMessage = src._serverMessage;
    return *this;
}

// Methods
int TcpServer::startServer(){
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0)
    {
        exitWithError("Cannot create socket");
        return 1;
    }
    if (bind(_serverSocket,(struct sockaddr *)&_serverSocketAddress, _socketAddressLen) < 0)
    {
        exitWithError("Cannot connect socket to address");
        return 1;
    }
    return 0;
}

void TcpServer::startListen()
{
    if (listen(_serverSocket, 5) < 0)
    {
        exitWithError("Socket listen failed");
    }
    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: " 
        << inet_ntoa(_serverSocketAddress.sin_addr) // convert the IP address (binary) in string
        << " PORT: " << ntohs(_serverSocketAddress.sin_port) // invert octets, beacause network have big endians before and we want to have little endians before
        << " ***\n\n";
    log(ss.str());
}

void TcpServer::acceptConnection(){
    char    buffer[1025];
    ssize_t bytesReceived;
    long    bytesSent;

    _clientSocket = accept(_serverSocket, (struct sockaddr *)&_serverSocketAddress, 
                &_socketAddressLen);
    if (_clientSocket < 0){
        std::ostringstream ss;
        ss << 
        "Server failed to accept incoming connection from ADDRESS: " 
        << inet_ntoa(_serverSocketAddress.sin_addr) << "; PORT: " 
        << ntohs(_serverSocketAddress.sin_port);
        exitWithError(ss.str());
    }
    bytesReceived = read(_clientSocket, buffer, sizeof(buffer) - 1);
    buffer[bytesReceived] = '\0';
    if (bytesReceived < 0)
        exitWithError("Failed to read bytes from client socket connection");
    bytesSent = write(_clientSocket, _serverMessage.c_str(), _serverMessage.size());
    if (bytesSent == _serverMessage.size())
        log("------ Server Response sent to client ------\n\n");
    else
        log("Error sending response to client");
}

void TcpServer::closeServer(){
    close(_serverSocket);
    close(_clientSocket);
    exit(0);
}