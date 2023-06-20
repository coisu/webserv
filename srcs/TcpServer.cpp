#include "TcpServer.hpp"
#include <stdio.h>
#include <errno.h>
#include "Request.hpp"
#include "CGI.hpp"

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
TcpServer::TcpServer():_port(8080){}

TcpServer::TcpServer( std::string ipAddress, int port ): _sIpAddress(ipAddress),
_port( port ), _serverSocket(), _clientSocket(), _serverIncomingMessage(),
 _socketSet(), _maxSocket(), _socketAddressLen( sizeof(_serverSocketAddress) ), _serverMessage(buildResponse(200, "hello worldywoo!")){
    setUpServer();
}

void    TcpServer::setUpServer(){

        startServer();
        _serverSocketAddress.sin_family = AF_INET; // for IPv4
        memset(&_serverSocketAddress.sin_zero, 0, sizeof(_serverSocketAddress.sin_zero));
        _serverSocketAddress.sin_port = htons(_port); // call htons to ensure that the port is stored in network byte order
        _serverSocketAddress.sin_addr.s_addr = INADDR_ANY; // is the address 0.0.0.0
        _timeout.tv_sec = 3 * 60;
        _timeout.tv_usec = 0;
        if (bind(_serverSocket,(struct sockaddr *)&_serverSocketAddress, _socketAddressLen) < 0)
            exitWithError("Cannot connect socket to address");//ATTENTION need to be manage correctly
        _maxSocket = _serverSocket;
        inet_addr(_sIpAddress.c_str()); // convert the IP address from a char * to a unsigned long and have it stored in network byte order
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
    this->_port = src._port;
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
    
    int ret;
    int option = 1;

    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0)
    {
        exitWithError("Cannot create socket");
        return 1;
    }
    ret = setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option));//Forcefully attaching socket to the port
    if (ret < 0)
    {
        exitWithError("bfrforce to reuse a port function setsockopt");
        return 1;
    }
    ret = fcntl(_serverSocket, F_SETFL, O_NONBLOCK);// Set socket to be nonblocking
    if (ret < 0)
    {
        exitWithError("Error function fcnt");
        return 1;
    }
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

    //     // New connexion is comming
        _clientSocket = accept(_serverSocket, (struct sockaddr *)&_serverSocketAddress, (socklen_t*)&_socketAddressLen);
        // std::cout << "client_socket" << _clientSocket << std::endl;
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
}

void    TcpServer::runServer(){
    
    ssize_t bytesReceived;
    long    bytesSent;
    fd_set  tempSet;
    int     activity;
	char	buffer[1024];
    // std::vector<int>    _fds;

    FD_ZERO(&tempSet);
    memcpy(&tempSet, &_socketSet, sizeof(_socketSet));
    activity = select(_maxSocket + 1, &tempSet, NULL, NULL, &_timeout);
    //_fds.push_back(select(_maxSocket + 1, &tempSet, NULL, NULL, &_timeout));
    if (activity == -1)//_fds.back() == -1)
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
                    // std::cout << "My buffer is: " << buffer << "\n\n"; 
                    bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
                    // std::cout << buffer << std::endl;
                    if (bytesReceived <= 0)
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
						// std::cout << buffer << std::endl;
						Request	request(buffer);
						// CGI		cgi(request);
						if (request.getCGI())
							request.getCGI()->getCharEnv();
                        bytesSent = send(socket, _serverMessage.c_str(), _serverMessage.size(), 0);
                        if (bytesSent == (long int)_serverMessage.size())
                            log("------ Server Response sent to client check------\n\n");
                        else
                            log("Error sending response to client");
                        close(socket);
                        FD_CLR(socket, &_socketSet);
                    }
                }
            }
        } 
}

void    TcpServer::closeServer(){
    close(_serverSocket);
    close(_clientSocket);
    exit(0);
}

/* ------------------------ Getters ------------------------------ */

int     TcpServer::getServerSocketFd(){ return(_serverSocket); }
int     TcpServer::getPort(){ return(_port); }

/* ------------------------ Setters ------------------------------ */

void    TcpServer::setPort(int port){ _port = port; }