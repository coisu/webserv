#include "../includes/http_tcpServer.h"
#include <stdio.h>
#include <errno.h>

void log(const std::string &message){
    std::cerr << message << std::endl;
}

void exitWithError(const std::string &errorMessage){
    log("ERROR: " + errorMessage);
    exit(1);
}

std::string buildResponse(int code, std::string body)
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << code << " OK\r\n"
          << "Content-Type: text/plain\r\n"
          << "Content-Length: " << body.size() << "\r\n"
          << "\n" << body << "\r\n";
    // std::cout << ss.str();
    return (ss.str());
}

// Constructor
TcpServer::TcpServer( std::string ipAddress, int port ): _sIpAddress(ipAddress),
_serverPort( port ), _serverSocket(), _clientSocket(), _serverIncomingMessage(),
 _socketSet(), _maxSocket(), _socketAddressLen( sizeof(_serverSocketAddress) ), _serverMessage(buildResponse(200, "hello worldywoo!")){
    _serverSocketAddress.sin_family = AF_INET; // for IPv4
    _serverSocketAddress.sin_port = htons(8080); // call htons to ensure that the port is stored in network byte order
    _serverSocketAddress.sin_addr.s_addr = INADDR_ANY; // is the address 0.0.0.0
    _timeout.tv_sec = 3 * 60;
    _timeout.tv_usec = 0;
    inet_addr(_sIpAddress.c_str()); // convert the IP address from a char * to a unsigned long and have it stored in network byte order
    startServer();
    startListen();
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
    this->_serverMessage = src._serverMessage;
    this->_maxSocket = src._maxSocket;
    this->_socketSet = src._socketSet;
    this->_serverIncomingMessage = src._serverIncomingMessage;
    this->_socketAddressLen = src._socketAddressLen;
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
    _maxSocket = _serverSocket;
    return 0;
}

void TcpServer::startListen()
{
    int yes = 1;

    if (listen(_serverSocket, 10) < 0)
        exitWithError("Socket listen failed");
    if (setsockopt(_serverSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    } 
    if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0)
        exitWithError("Socket non-blocking failed");
    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: " 
        << inet_ntoa(_serverSocketAddress.sin_addr) // convert the IP address (binary) in string
        << " PORT: " << ntohs(_serverSocketAddress.sin_port) // invert octets, beacause network have big endians before and we want to have little endians before
        << " ***\n\n";
    log(ss.str());
    FD_ZERO(&_socketSet);
    FD_SET(_serverSocket, &_socketSet);
}

void TcpServer::acceptConnection(){

    // if (FD_ISSET(_serverSocket, &_socketSet)){
    //     // New connexion is comming
        _clientSocket = accept(_serverSocket, (struct sockaddr *)&_serverSocketAddress, (socklen_t*)&_socketAddressLen);
        std::cout << "client_socket" << _clientSocket << std::endl;
        if (_clientSocket < 0){
            std::ostringstream ss;
            ss << 
            "Server failed to accept incoming connection from ADDRESS: " 
            << inet_ntoa(_serverSocketAddress.sin_addr) << "; PORT: " 
            << ntohs(_serverSocketAddress.sin_port);
            exitWithError(ss.str());
        }
        // if (fcntl(_clientSocket, F_SETFL, O_NONBLOCK) < 0)
        //     exitWithError("webserv: fcntl error");
        // + new socket
        FD_SET(_clientSocket, &_socketSet);
        if (_clientSocket > _maxSocket)
            _maxSocket = _clientSocket;

        std::cout << "New connexion comming: " << inet_ntoa(_serverSocketAddress.sin_addr) << ":" << ntohs(_serverSocketAddress.sin_port) << std::endl;
    // }
}

void    TcpServer::runServer(){
    char    buffer[1024] = {0};
    ssize_t bytesReceived;
    long    bytesSent;
    fd_set  tempSet;
    int     activity;

    FD_ZERO(&tempSet);
    memcpy(&tempSet, &_socketSet, sizeof(_socketSet));
    activity = select(_maxSocket + 1, &tempSet, NULL, NULL, &_timeout);
    if (activity == -1)
        exitWithError("Error calling select()");
    if (activity == 0) {
        exitWithError("Timeout reached, no activity on sockets");
        return;
    }
    for (int socket = 0; socket <= _maxSocket; socket++){
        // std::cout << "max_socket" << _maxSocket << std::endl;
            if (FD_ISSET(socket, &tempSet)) {
                if (socket == _serverSocket){
                    // memset(buffer, 0, sizeof(buffer));
                    acceptConnection();
                    //accept connection
                } else {
                    std::cout << "RECEIVING DATA FROM CLIENT: " << socket << std::endl;
                    if ((bytesReceived = recv(socket, buffer, sizeof(buffer), 0)) <= 0)
                    {
                        if (bytesReceived == 0)
                        {
                            std::cout << "connection closed from socket: " << socket << std::endl;
                        }
                        else {
                            std::cout << "error with socket: " << socket << std::endl;
                        }
                        close(socket);
                        FD_CLR(socket, &_socketSet);
                    } else {
                        std::cout << "we got data" << std::endl;
                        bytesSent = send(socket, _serverMessage.c_str(), _serverMessage.size(), 0);
                        if (bytesSent == (long int)_serverMessage.size())
                            log("------ Server Response sent to client ------\n\n");
                        else
                            log("Error sending response to client");

                        // for (int socketSent = 0; socketSent <= _maxSocket; socketSent++){
                        //     if (FD_ISSET(socketSent, &_socketSet)){
                        //         if (socketSent != _serverSocket && socketSent != socket){
                        //             std::cout << _serverMessage.c_str();
                        //             bytesSent = send(socket, _serverMessage.c_str(), _serverMessage.size(), 0);
                        //             if (bytesSent == (long int)_serverMessage.size())
                        //                 log("------ Server Response sent to client ------\n\n");
                        //             else
                        //                 log("Error sending response to client");
                        //         }
                        //     }
                        // }
                    }
                    // bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
                    // std::cout << bytesReceived << " " << sizeof(buffer) << std::endl;
                    // if (bytesReceived <= 0){
                    //     if (bytesReceived == 0){
                    //         // perror("Erreur lors de la réception des données 2");
                    //         std::ostringstream ss;
                    //         ss << "Desconexion of socket: " << socket << ": " << buffer;
                    //         exitWithError(ss.str());
                    //     } else
                    //         perror("Erreur lors de la réception des données");
                    //         // exitWithError("Failed to read bytes from client socket connection");
                    //     close(socket);
                    //     FD_CLR(socket, &_socketSet);       
                    // } else {
                    //     std::cout << "socket" << socket << std::endl;
                    //     std::cout << "Data received from the socket " << socket << ": " << buffer << std::endl;
                    //     for (int socketSent = 0; socketSent <= _maxSocket; socketSent++){
                    //         if (FD_ISSET(socketSent, &_socketSet)){
                    //             if (socketSent != _serverSocket && socketSent != socket){
                    //                 std::cout << _serverMessage.c_str();
                    //                 bytesSent = send(socket, _serverMessage.c_str(), _serverMessage.size(), 0);
                    //                 if (bytesSent == (long int)_serverMessage.size())
                    //                     log("------ Server Response sent to client ------\n\n");
                    //                 else
                    //                     log("Error sending response to client");
                    //             }
                    //         }
                    //     }
                    // } 
                }
            }
        } 
}

void    TcpServer::closeServer(){
    close(_serverSocket);
    close(_clientSocket);
    exit(0);
}