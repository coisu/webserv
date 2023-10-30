#include "Server.hpp"

// #include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "Request.hpp"
#include "Response.hpp"
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
          << "Content-Type: text/html\r\n" //text/plain\r\n"
          << "Content-Length: " << body.size() << "\r\n"
          << "\n" << body << "\r\n";
    // std::cout << ss.str();
    return (ss.str());
}

// void	Server::initDefaults()
// {
// 	this->_port = 0;
// 	this->_host = 0;
// 	this->_serverName = "localhost";
// 	// this->_errorPages = ;
// 	this->_clientBodySize = std::numeric_limits<size_t>::max();
// 	this->_root = "";
// 	// this->_index = "";
// 	// this->_autoIndex = false;
// 	// this->_locations = ;
// 	this->_listenFd = 0;
// 	this->_block = "";
// 	this->_sIpAddress = "";
// 	//this->_serverPort = 0;
// 	this->_serverSocket = 0;
// 	this->_clientSocket = 0;
// 	this->_serverIncomingMessage = 0;
// 	// this->_socketSet = 0;
// 	this->_maxSocket = 0;
// 	// this->_serverSocketAddress = ;
// 	this->_socketAddressLen = 0;
// 	this->_serverMessage = "";
// 	// this->_timeout = ;
// }

Server::Server( void ) {}

Server::~Server( void ) {}

Server::Server(std::string serverBlock, std::vector<Location> locationVec)
{
	// initDefaults();
    std::stringstream   ss(serverBlock);
    std::string         part;

    this->_locations = locationVec;
    this->_block = serverBlock;

	//this->_serverPort = this->_port;
	// this->_serverSocket = ();
	// this->_clientSocket = ();
	// this->_serverIncomingMessage = (),
	// this->_socketSet = ();
	// this->_maxSocket = 
	// this->_port = 0;
	this->_host = 0;
	this->_serverName = "";
	this->_clientBodySize = 0;
	this->_root = "";
	this->_listenFd = 0;
	this->_block = "";
	this->_sIpAddress = "";
	this->_serverSocket = 0;
	this->_clientSocket = 0;
	this->_serverIncomingMessage = 0;
	this->_maxSocket = 0;
    // this->_serverSocketAddress.sin_family = AF_INET; // for IPv4
    // this->_serverSocketAddress.sin_port = this->_port;
    // this->_serverSocketAddress.sin_addr.s_addr = INADDR_ANY; // is the address 0.0.0.0
	this->_socketAddressLen = sizeof(_serverSocketAddress);
	this->_serverMessage = buildResponse(200, "hello worldywoo!");
    this->_timeout.tv_sec = 3 * 60;
    this->_timeout.tv_usec = 0;
    // inet_addr(this->_sIpAddress.c_str()); // convert the IP address from a char * to a unsigned long and have it stored in network byte order
    // startServer();
    // startListen();

    /*Fill Server Values*/
    while (std::getline(ss, part, ';'))
    {
        std::string key = part.substr(0, part.find(':'));
        std::string value = part.substr(part.find(':')+1);
        setAttributes(key, value);
    }
    // std::cout << "SERVER:\n" << *this << std::endl;
}

void    Server::setAttributes(std::string key, std::string value)
{
    size_t      N = 6;

    std::string keys[6] = {"port", 
                          "host", 
                          "server_name", 
                          "error_page", 
                          "client_body_size", 
                          "root"}; 
                        //   "index", 
                        //   "autoindex"};
    size_t i = 0;
    while (i < N && keys[i] != key)
        i++;
    switch (i)
    {
    case 0:
        initPort(value);
        break;
    case 1:
        initHost(value);
        break;
    case 2:
        initServerName(value);
        break;
    case 3:
        initErrorPage(value);
        break;
    case 4:
        initClientBodySize(value);
        break;
    case 5:
        initRoot(value);
        break;
    // case 6:
    //     initIndex(value);
    //     break;
    // case 7:
    //     initAutoIndex(value);
    //     break;
    default:
        throw std::runtime_error("unrecognised server key: " + key);
    }
}

Server::Server( const Server& src )
{
    this->_port = src._port;
    this->_host = src._host;
    this->_serverName = src._serverName;
    this->_errorPages = src._errorPages;
    this->_clientBodySize = src._clientBodySize;
    this->_root = src._root;
    // this->_index = src._index;
    // this->_autoIndex = src._autoIndex;
    this->_locations = src._locations;
    this->_listenFd = src._listenFd;
    this->_block = src._block;

	this->_sIpAddress = src._sIpAddress;
	//this->_serverPort = src._serverPort;
	this->_serverSocket = src._serverSocket;
	this->_clientSocket = src._clientSocket;
	this->_serverIncomingMessage = src._serverIncomingMessage;
	this->_socketSet = src._socketSet;
	this->_maxSocket = src._maxSocket;
	this->_serverSocketAddress = src._serverSocketAddress;
	this->_socketAddressLen = src._socketAddressLen;
	this->_serverMessage = src._serverMessage;
	this->_timeout = src._timeout;
}

Server& Server::operator=( const Server& src )
{
    if (this != &src)
	{
        this->_port = src._port;
        this->_host = src._host;
        this->_serverName = src._serverName;
        this->_errorPages = src._errorPages;
        this->_clientBodySize = src._clientBodySize;
        this->_root = src._root;
        // this->_index = src._index;
        // this->_autoIndex = src._autoIndex;
        this->_locations = src._locations;
        this->_listenFd = src._listenFd;
        this->_block = src._block;

		this->_sIpAddress = src._sIpAddress;
		//this->_serverPort = src._serverPort;
		this->_serverSocket = src._serverSocket;
		this->_clientSocket = src._clientSocket;
		this->_serverIncomingMessage = src._serverIncomingMessage;
		this->_socketSet = src._socketSet;
		this->_maxSocket = src._maxSocket;
		this->_serverSocketAddress = src._serverSocketAddress;
		this->_socketAddressLen = src._socketAddressLen;
		this->_serverMessage = src._serverMessage;
		this->_timeout = src._timeout;
    }
	return (*this);

}

// Methods
int Server::startServer(){
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
    std::cout << "< max socket > " << _maxSocket <<std::endl;
    return 0;
}

void Server::startListen()
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

void Server::acceptConnection(){

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

void    Server::runServer(){
    
    ssize_t bytesReceived;
    long    bytesSent;
    fd_set  tempSet;
    int     activity;
	char	buffer[10024];

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

						// Request request = process_request(buffer);
						try
						{
							// Request	request(buffer, *this);
                            // std::string requestUrl = request.getURL();
                            // CGI*    cgiTest = NULL;
                            // if (requestUrl.find("cgi-bin") != requestUrl.npos)
                            // {
                            //     std::map<std::string, std::string>  cgiConfig;
                            //     cgiConfig["/usr/bin/python3"] = ".py";
                            //     cgiConfig["/bin/bash"] = ".sh";
                            //     cgiTest = new CGI(*this, requestUrl, request.getMethodStr(), cgiConfig);
                            // }
							// if (request.getCGI())
							// {
							// 	request.getCGI()->getCharEnv();
							// 	std::string	cgi_response = request.getCGI()->exec_cgi();
							// 	// std::string	cgi_response = EXAMPLE_RESPONSE;
							// 	bytesSent = send(socket, cgi_response.c_str(), cgi_response.size(), 0);
							// 	// std::cout << "\n" << bytesSent << "RESPONSE: " << cgi_response << std::endl;
							// 	// while(true);
							// }
							// else
							// {


								// std::string	msg = buildResponse(200, "JELLOO\n");
                                // msg = buildResponse(200, readFile("bing.html"));
                                // if (cgiTest)
                                //     msg = buildResponse(200, cgiTest->exec_cgi());
								// bytesSent = send(socket, msg.c_str(), msg.size(), 0);



							// }
								// bytesSent = send(socket, _serverMessage.c_str(), _serverMessage.size(), 0);
							// std::cout << "\n servmsg: " << _serverMessage << std::endl;
							// if (bytesSent == (long int)_serverMessage.size())
								// log("------ Server Response sent to client check------\n\n");
							// else
								// log("Error sending response to client");
              
                            // request.printRequest();
                            std::string responseBuffer;
                            // Response response(request, *this);

                            // responseBuffer = response.processResponse();

                            // std::cout << "\n\n\n >> Response result buffer PRINT >>>>> \n\n" << responseBuffer <<std::endl;
                            // bytesSent = send(socket, responseBuffer.c_str(), responseBuffer.size(), 0);
                            
                            // response.clear();
						}
						catch(int errcode)
						{
							// std::string	code(SSTR(errcode));
							// std::cerr << "error code: " << errcode << std::endl;
							// std::string	errorbod(buildResponse(404, "error code: " + SSTR(errcode)));

                            // Request	request(buffer, *this);

                            // std::string responseBuffer;
                            // Response response(request, *this);

                            // response.setStatus(errcode);
                            // responseBuffer = response.processResponse();
                            // bytesSent = send(socket, responseBuffer.c_str(), responseBuffer.size(), 0);
                            
                            // response.clear();
							// bytesSent = send(socket, errorbod.c_str(), errorbod.size(), 0);
						}
						
						(void)bytesSent;
                        close(socket);
                        FD_CLR(socket, &_socketSet);
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

void    Server::closeServer(){
    close(_serverSocket);
    close(_clientSocket);
    exit(0);
}

std::ostream& operator<<(std::ostream& os, const Server& server)
{
    // os << "Server Info:" << std::endl;
    os << "port: " << server._port << std::endl;
    os << "host: " << server._host << std::endl;
    os << "serverName: " << server._serverName << std::endl;
    os << "errorPages: [";
    for (std::map<int, std::string>::const_iterator it = server._errorPages.begin(); it != server._errorPages.end(); it++)
        os << it->first << ", " << it->second << " | ";
    os << "]\n";
    os << "client body size: " << server._clientBodySize << std::endl;
    os << "root: " << server._root << std::endl;
    // os << "index: " << server._index << std::endl;
    // os << "autoIndex: " << server._autoIndex << std::endl;
    os << "------------LOCATIONS-------------\n";
    for (size_t i = 0; i < server._locations.size(); i++)
        os << "location " << i + 1 << ":\n" << server._locations[i] << std::endl;
    os << "----------------------------------\n";
    // os << "listenFd " << server._listenFd << std::endl;
    // os << "block " << server._block << std::endl;    
    return os;
}

void    Server::setLocations( std::vector<Location> locationVec )
{
    this->_locations = locationVec;
}

void Server::initPort(std::string value)
{
    if (value.find_first_not_of("0123456789") != value.npos || value.size() > 5)
        throw std::runtime_error("invalid port in server block.");
    int val = atoi(value.c_str());
    if (val < 1 || val > 65535)
        throw std::runtime_error("port must be between 1 and 65,535.");
    this->_port = htons(val);
}

void Server::initHost(std::string value)
{
  	std::string part;
	std::string ip[4];
	std::stringstream ss(value);
    int i = 0;
    if (value.find_first_not_of("0123456789.") != value.npos || value.size() > 15)
        throw std::runtime_error("(1) invalid host address in server block.");
    while (std::getline(ss, part, '.'))
    {
        if (i == 4)
            throw std::runtime_error("(2) invalid host address in server block.");
        ip[i++] = part;
    }
    for (int j = 0; j < 4; j++)
    {
        if (ip[j].size() == 0)
            throw std::runtime_error("(3) invalid host address in server block.");
        if (atoi(ip[j].c_str()) > 255 || atoi(ip[j].c_str()) < 0)
            throw std::runtime_error("(4) invalid host address in server block.");
    }
    this->_host = inet_addr(value.c_str());
	std::cerr << "INITHOST: host: " << this->_host << std::endl;
}

void Server::initServerName(std::string value)
{
    this->_serverName = value;
}

void Server::initErrorPage(std::string value)
{
    std::stringstream   ss(value);
    std::string         code;
    std::string         path;
    
    while (std::getline(ss, code, ',') && std::getline(ss, path, ','))
    {
        if (code.size() > 3 || code.find_first_not_of("0123456789") != code.npos)
            throw std::runtime_error("invalid error page code.");
        int stat = atoi(code.c_str());
        if (stat > 599 || stat < 400)
            throw std::runtime_error("error codes must be in the range: 400-599.");
        if (path[0] != '/')
            throw std::runtime_error("error page path must start with \'/\' - path: " + path);
        if (path.at(path.length() - 1) == '/' && path.size() > 1)
            throw std::runtime_error("error page path must *not* end with \'/\' - path: " + path);
        this->_errorPages[stat] = path;
    }
}

void Server::initClientBodySize(std::string value)
{
    if (value.size() > 18 || value.find_first_not_of("0123456789") != value.npos)
        throw std::runtime_error("bad client body size.");
    this->_clientBodySize = static_cast<size_t>(atoll(value.c_str()));
}

void Server::initRoot(std::string value)
{
    if (value.at(value.length() - 1) == '/' && value.size() > 1)
        throw std::runtime_error("root path must *not* end with \'/\': path: " + value);
    if (pathIsDir(value) != IS_DIR)
        throw std::runtime_error("root path is not a valid directory: path: " + value);
    // if (!pathExists(value))
    //     throw std::runtime_error("bananaaaaaaaaaaaaa");
	// things
    this->_root = value;
}

// void Server::initIndex(std::string value)
// {
//     this->_index = value;
// }

// void Server::initAutoIndex(std::string value)
// {
//     this->_autoIndex = (value == "on") ? true : false;
// }

unsigned int	Server::getPort() const
{
	return (this->_port);
}

in_addr_t	Server::getHost() const
{
	return (this->_host);
}

std::string	Server::getServerName() const
{
	return (this->_serverName);
}

std::map<int, std::string>	Server::getErrorPages() const
{
	return (this->_errorPages);
}

size_t	Server::getClientBodySize() const
{
	return (this->_clientBodySize);
}

std::string	Server::getRoot() const
{
	return (this->_root);
}

// std::string	Server::getIndex() const
// {
// 	return (this->_index);
// }

// bool	Server::getAutoIndex() const
// {
// 	return (this->_autoIndex);
// }

std::vector<Location>	Server::getLocations() const
{
	return (this->_locations);
}

int	Server::getListenFd() const
{
	return (this->_listenFd);
}

std::string	Server::getBlock() const
{
	return (this->_block);
}

#include <iostream>
const std::pair<bool, Location> Server::srchLocation(std::string& path) const
{
    Location    ret;
    size_t match = 0;

    std::vector<Location>::const_iterator cur = _locations.begin();
    std::vector<Location>::const_iterator end = _locations.end();

    unsigned long p;
    int idx;

    for (; cur != end; ++cur)
    {
        p = path.find(cur->getPath());
        if (p != std::string::npos)
        {
            if(cur->getPath().length() > match)
            {                
                idx = p + cur->getPath().length();
                if (path[idx] == '/' || path[idx] == '\0')
                {
                    match = cur->getPath().length();
                    int tmp = static_cast<int>(match);
                    std::stringstream ssIn;
                    ssIn << tmp;
                    ret = *cur;
                }
            }
        }
    }
    if (match != 0)
    {
        return (std::make_pair(true, ret));
    }
	return (std::make_pair(false, ret)); 
}