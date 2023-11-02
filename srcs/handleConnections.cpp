#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <queue>
#include <fstream>
#include <sstream>
#include <signal.h>

#include "utils.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "CGI.hpp"

struct ClientState {
	std::string	incompleteRequest; // buffer for storing partial HTTP request
	std::string	incompleteResponse; // buffer for storing partial HTTP response
	bool		isCGI;
	bool		isChunked;
	bool		isReceivingBody;
	bool		isClosing;
	bool		isClosed;
	bool		requestCompleted;
	size_t		contentLength;
	size_t		receivedLength;
	std::map<std::string, std::string>	header;
	std::string							body;
	std::string							info;
	std::queue<std::string>				responseQueue;
	// ... other states like request headers, method, etc.
};

struct CgiState {
	std::string incompleteResponse;
	bool		isFinished;
	int			clientSocket;
};

void    parseHttpRequest(ClientState &client)
{
	size_t pos = client.incompleteRequest.find("\r\n\r\n"); // <-- seach for header-body seperator
	if (!client.isReceivingBody && pos != std::string::npos) // <-- if client isnt receiving body and seperator is found
	{
		std::istringstream  iss(client.incompleteRequest.substr(0, pos + 4)); // <-- put header part of the request into istream
		std::string         key, val;

		std::getline(iss, client.info); // <-- put request first line into info to be passed later
		client.info = trimWhiteSpace(client.info);
		// parse header into map of key: values
		while(std::getline(std::getline(iss, key, ':') >> std::ws, val))
			client.header[trimWhiteSpace(key)] = trimWhiteSpace(val.substr(0, val.size() - 1));

		client.incompleteRequest.erase(0, pos + 4); // <-- erase what has been parsed

		// if Content-Length is present in header setup values to start receiving the body
		if (client.header["Content-Length"].empty())
			client.requestCompleted = true;
		else
		{
			client.contentLength = atoi(client.header["Content-Length"].c_str());
			client.isReceivingBody = true;
			client.receivedLength = 0;
		}
	}
	size_t i = 0;
	// put all the available request body in client.body
	while (client.receivedLength < client.contentLength && i < client.incompleteRequest.size())
	{
		client.body += client.incompleteRequest[i];
		client.receivedLength++;
		i++;
	}
	client.incompleteRequest.erase(0, i); // <-- erase what has been saved
	if (client.receivedLength == client.contentLength) // <-- check if the body has been fully received
	{
		if (client.isReceivingBody == true)
			client.requestCompleted = true;
		client.isReceivingBody = false;
	}
}

std::string    printClient(const ClientState &client)
{
	std::ostringstream os;
	std::map<std::string, std::string>::const_iterator it;
	os << "INFO:" << std::endl << client.info << ";" << std::endl;
	os << "\nHEADER:" << std::endl;
	for (it = client.header.begin(); it != client.header.end(); it++)
	{
		os << "\"" << it->first << ": " << it->second << "\";" << std::endl;
	}
	os << "\nBODY:" << std::endl;
	os << "\"" << client.body << "\";" << std::endl;

	return (os.str());
}

std::string    printRequest(const Request &request)
{
	std::ostringstream os;
	std::map<std::string, std::string>::const_iterator it;
	
	os << request.getInfo() << ";\n";
	for (it = request.getHead().begin(); it != request.getHead().end(); it++)
	{
		os << it->first << ": " << it->second << ";" << std::endl;
	}
	os << "\n" << std::endl;
	os << request.getBody() << ";" << std::endl;

	return (os.str());
}

std::string makeResponse(int code, std::string body)
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << code << " OK\r\n"
		  << "Content-Type: text/plain\r\n" //text/plain\r\n"
		  << "Content-Length: " << body.size() << "\r\n"
		  << "\n" << body << "\r\n";
	// std::cout << ss.str();
	return (ss.str());
}

int chooseServer(int clientSocket, ClientState client, std::vector<Server> &servers)
{
	int	serverIndex = -1;
	std::vector<Server>::iterator it;

	// get the port of the connected client with clientSocket and getsockname()
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(clientSocket, (struct sockaddr *)&sin, &len) == -1)
	{
		ft_logger("Error getting client port", ERROR, __FILE__, __LINE__);
		throw 500;
	}
	else
		ft_logger("Client port (" + SSTR(ntohs(sin.sin_port)) + ") received", INFO, __FILE__, __LINE__);

	// set the client values
	unsigned int clientPort = sin.sin_port;
	std::string clientName = client.header["Host"];
	clientName = clientName.substr(0, clientName.find(':'));

	// The first server for a host:port will be the{...} default for this host:port
	for (it = servers.begin(); it != servers.end(); it++)
	{
		if (it->getPort() == clientPort)
		{
			serverIndex = it - servers.begin();
			break ;
		}
	}

	// compare the client values with the server values to match the right server
	for (it = servers.begin(); it != servers.end(); it++)
	{
		if (it->getPort() == clientPort && it->getServerName() == clientName)
		{
			serverIndex = it - servers.begin();
			break ;
		}
	}

	// return NULL if no server was found and there are no default servers (servers with no name)
	return (serverIndex);
}

void signalHandler(int signum)
{
	(void)signum;
	std::string msg = "\nTerminating server...";
	std::cout << msg << std::endl;
	global_running_flag = 0; // Set the flag to break out of the loop
}

void	recvSendLoop(std::vector<int> &serverSockets, int &maxSocket, std::vector<Server> &servers)
{
	fd_set  readSet, writeSet;
	std::map<int, ClientState>				clients;
	std::map<int, ClientState>::iterator	it;
	std::map<int, CgiState>					cgi_map;
	std::map<int, CgiState>::iterator		cgi_it;

	signal(SIGINT, signalHandler);

	while (global_running_flag)
	{
		ssize_t bytesReceived = 0;
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);

		// Add all server sockets to the set.
		for (size_t i = 0; i < serverSockets.size(); i++)
			FD_SET(serverSockets[i], &readSet);

		// Add all cgis to the set.
		for (cgi_it = cgi_map.begin(); cgi_it != cgi_map.end(); cgi_it++)
			FD_SET(cgi_it->first, &readSet);

		// Add all client sockets to writeSet and readSet
		for (it = clients.begin(); it != clients.end(); )
		{
			int clientSocket = it->first;
			ClientState &client = it->second;

			if (client.isClosed) // <-- if client is closed then erase from clients
				clients.erase(it++);
			else if (client.isClosing // <-- if client is closing and there is no more data to send then close
			&& client.incompleteResponse.empty()
			&& client.responseQueue.empty())
			{
				close(clientSocket);
				clients.erase(it++);
			}
			else if (fcntl(clientSocket, F_GETFD) == -1) // <-- if client is broken or closed then erase from clients
			{
				close(clientSocket);
				clients.erase(it++);
			}
			else
			{
				FD_SET(clientSocket, &readSet);
				FD_SET(clientSocket, &writeSet);
				it++;
			}
		}

		for (int i = 0; i < FD_SETSIZE; ++i) {
			if (FD_ISSET(i, &readSet)) {
				if (fcntl(i, F_GETFD) == -1) {
					// Invalid file descriptor
					perror("fcntl read");
				}
			}
			if (FD_ISSET(i, &writeSet)) {
				if (fcntl(i, F_GETFD) == -1) {
					
					perror("fcntl write");
				}
			}
		}

		// wait for activity on sockets in readSet and writeSet
		if (select(maxSocket + 1, &readSet, &writeSet, NULL, NULL) == -1)
		{
			// throw std::runtime_error("Select() failed");
			if (global_running_flag == true)
			{
				// exit(1);
				perror("select"); // <-- COMMENT THIS OUT LATER
				ft_logger("Error: select() failed", ERROR, __FILE__, __LINE__);
			}
			continue ;
		}
		// Loop through the server sockets to find the one that is ready.
		for (size_t i = 0; i < serverSockets.size(); i++)
		{
			// check the server socket to see if its ready
			if (FD_ISSET(serverSockets[i], &readSet))
			{
				// Accept and handle the connection on the port i.
				struct sockaddr_in  clientSocketAddress;
				socklen_t           clientSocketSize = sizeof(clientSocketAddress);

				// accept the client connection and create client socket
				int clientSocket = accept(serverSockets[i], (struct sockaddr *)&clientSocketAddress, &clientSocketSize);
				if (clientSocket < 0)
				{
					std::stringstream ss;
					ss << "Server failed to accept incoming connection from clent ADDRESS: " 
					<< inet_ntoa(clientSocketAddress.sin_addr) << "; PORT: " 
					<< ntohs(clientSocketAddress.sin_port);
					ft_logger(ss.str(), ERROR, __FILE__, __LINE__);
					continue ;
				}

				// update maxSocket as needed
				if (clientSocket > maxSocket)
					maxSocket = clientSocket;

				clients[clientSocket] = (ClientState){std::string(), std::string(), false, false, false, false, false, false, 0, 0, 
													  std::map<std::string, std::string>(), std::string(), std::string(), std::queue<std::string>()};

				std::stringstream ss;
				ss	<< "New connection incomming: " 
					<< inet_ntoa(clientSocketAddress.sin_addr) 
					<< ":" << ntohs(clientSocketAddress.sin_port) 
					<< std::endl;
				ft_logger(ss.str(), INFO, __FILE__, __LINE__);
			}
		}

		// Loop through cgi_map to check for readiness for reading
		for (cgi_it = cgi_map.begin(); cgi_it != cgi_map.end(); )
		{
			int	cgiSocket = cgi_it->first;
			CgiState &cgi = cgi_it->second;

			if (FD_ISSET(cgiSocket, &readSet))
			{
				char	buffer[1024];

				bytesReceived = read(cgiSocket, buffer, sizeof(buffer));
				if (bytesReceived < 0)
				{
					ft_logger("Error reading from CGI", ERROR, __FILE__, __LINE__);
					close(cgiSocket);
					cgi_map.erase(cgi_it++);
					continue ;
				}
				else if (bytesReceived == 0)
				{
					ft_logger("CGI pipe read returned zero", INFO, __FILE__, __LINE__);
					close(cgiSocket);
					clients[cgi.clientSocket].responseQueue.push(cgi.incompleteResponse);
					cgi.incompleteResponse.clear();
					cgi_map.erase(cgi_it++);
					continue ;
				}
				else
				{
					ft_logger("Reading from cgi..", INFO, __FILE__, __LINE__);
					cgi.incompleteResponse.append(buffer, bytesReceived); // <-- append received data
				}
			}
			cgi_it++;
		}

		// Loop through clients and check for readiness for reading and writing
		for (it = clients.begin(); it != clients.end(); it++)
		{
			// naming for clarity
			int         clientSocket = it->first;
			ClientState &client = it->second;

			if (FD_ISSET(clientSocket, &readSet)) // <-- check if client is ready to read from
			{
				char    buffer[1024];
				bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
				// std::cout << "\n----MESSGE----\n" << client.incompleteRequest << "\n-----END-----\n";
				if (bytesReceived < 0)
				{
					std::stringstream ss;
					ss << "Error reading from client: (" << clientSocket << ") " << std::endl;
					ft_logger(ss.str(), ERROR, __FILE__, __LINE__);
					close(clientSocket);
					FD_CLR(clientSocket, &readSet);
					FD_CLR(clientSocket, &writeSet);
					client.isClosed = true;
					perror("bytes");
					continue ;
				}
				else if (bytesReceived == 0)
				{
					std::cout << "RECV RETURNED ZERO\n";
					client.isClosing = true;
				}
				else
				{
					// std::string fullResponseStr;
					client.incompleteRequest.append(buffer, bytesReceived); // <-- append received data
					parseHttpRequest(client); // <-- parse the request into sdt::map client.header and std::string client.body
					if (client.requestCompleted == true) // <-- if the request has been fully parsed then create the response
					{
						try
						{
							int idx = chooseServer(clientSocket, client, servers); // <-- select correct host according to hostname and server port
							if (idx < 0 || (size_t)idx >= servers.size())
							{
								ft_logger("Error: server not found", ERROR, __FILE__, __LINE__);
								throw 404;
							}
							Request request(client.header, client.body, client.info, servers[idx]); // <-- create request obj with ClientStatus info
							Response response(request, servers[idx]); // <-- create response with request obj and selected server
							try
							{
								int fd = -1, pid = -1;
								std::string fullResponseStr = response.processResponse(fd, pid);
								if (fd > 0) // <-- if fd is greater than 0 then it is a cgi
									cgi_map[fd] = (CgiState){std::string(), false, clientSocket};
								// update maxSocket as needed
								if (fd > maxSocket)
									maxSocket = fd;
								if (!fullResponseStr.empty()) // <-- if the response was created (not cgi) push to the queue
									client.responseQueue.push(fullResponseStr);
							}
							catch(int errorCode) // <-- if there was an error creating the cgi then send error response
							{
								std::string body = response.buildErrorBody(errorCode);
								std::string header = response.buildHeader(body.size(), errorCode);
								client.responseQueue.push(header + body);
							}
							catch(...) // <-- if there was an error creating the cgi then send error response
							{
								std::string body = response.buildErrorBody(500);
								std::string header = response.buildHeader(body.size(), 500);
								client.responseQueue.push(header + body);
							}
							client.requestCompleted = false; // <-- set to false so that next message will be read
						}
						catch(int errorCode) // <-- if there was an error creating the cgi then send error response
						{
							std::string body = makeResponse(errorCode, "Error");
							client.responseQueue.push(body);
						}
						catch(...) // <-- if there was an error creating the cgi then send error response
						{
							std::string body = makeResponse(500, "Error");
							client.responseQueue.push(body);
						}
					}
				}
			}
			if (FD_ISSET(clientSocket, &writeSet) && client.responseQueue.empty() == false) // <-- check if client is ready to write into
			{
				std::string	&responseStr = client.responseQueue.front();
				int bytesSent = send(clientSocket, responseStr.data(), responseStr.size(), 0);
				if(bytesSent < 0)
				{
					// Handle disconnect
					close(clientSocket);
					FD_CLR(clientSocket, &readSet);
					FD_CLR(clientSocket, &writeSet);
					client.isClosed = true;
				}
				else if (bytesSent == 0)
				{
					// Handle disconnect
					close(clientSocket);
					FD_CLR(clientSocket, &readSet);
					FD_CLR(clientSocket, &writeSet);
					client.isClosed = true;
				}
				else
				{
					responseStr.erase(0, bytesSent); // <-- erase sent data
					if (responseStr.empty())
					{
						client.responseQueue.pop();
						// close(clientSocket);
						// client.isClosed = true;
					}
				}
			}
		}
	}
	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);
	// close all cgi fds
	for (cgi_it = cgi_map.begin(); cgi_it != cgi_map.end(); cgi_it++)
		close(cgi_it->first);
	// close all client sockets
	for (it = clients.begin(); it != clients.end(); it++)
		close(it->first);
	// close all server sockets
	for (size_t i = 0; i < serverSockets.size(); i++)
		close(serverSockets[i]);
}

std::set<int> getPorts(const std::vector<Server>& servers)
{
	std::set<int> ports;

	for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end(); ++it)
	{
		ports.insert(it->getPort());
	}
	
	return ports;
}

void handleConnections(std::vector<Server> &servers)
{
	//const std::vector<int> portVec = getPorts(servers);  // Get all the different ports in the servers
	const std::set<int> portSet = getPorts(servers);  // Get all the different ports in the servers

	// Create a vector to store socket descriptors for multiple ports.
	std::vector<int>    serverSockets;
	int                 maxSocket = 0;

	// Create and initialize sockets for each port.
	// for (size_t i = 0; i < portVec.size(); i++)
	for (std::set<int>::iterator it = portSet.begin(); it != portSet.end(); it++)
	{
		int port = *it;
		int currentSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (currentSocket == -1) {
			std::stringstream ss;
			ss << "Error creating socket for port " << port << std::endl;
			ft_logger(ss.str(), ERROR, __FILE__, __LINE__);
			throw std::runtime_error("Cannot create socket");
		}
		serverSockets.push_back(currentSocket);

		// Set up the server address structure.
		struct sockaddr_in  serverSocketAddress;
		serverSocketAddress.sin_family = AF_INET; // for IPv4
		// serverSocketAddress.sin_port = portVec[i];
		serverSocketAddress.sin_port = port;
		serverSocketAddress.sin_addr.s_addr = INADDR_ANY;

		ft_logger("PORT: " + SSTR(ntohs(port)) + "", DEBUG, __FILE__, __LINE__);
		
		// Set socket options to reuse the address and port.
		int yes = 1;
		if (setsockopt(currentSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
			ft_logger("Failed to setsocket: " + SSTR(currentSocket), ERROR, __FILE__, __LINE__);
			perror("setsockopt"); // <-- COMMENT THIS OUT LATER
		}

		// Bind the socket to the address and port.
		if (bind(currentSocket,(struct sockaddr *)&serverSocketAddress, sizeof(serverSocketAddress)) < 0)
		{
			close(currentSocket);
			perror("bind failed"); // <-- COMMENT THIS OUT LATER
			ft_logger("Failed to bind socket: " + SSTR(htons(serverSocketAddress.sin_port)), ERROR, __FILE__, __LINE__);
			throw std::runtime_error("Cannot bind socket to address");
		}

		// Listen for incoming connections.
		if (listen(currentSocket, 10) < 0)
		{
			close(currentSocket);
			ft_logger("Failed to listen on socket: " + SSTR(htons(serverSocketAddress.sin_port)), ERROR, __FILE__, __LINE__);
			throw std::runtime_error("Socket listen failed");
		}

		std::ostringstream ss;
		ss << "\n*** Listening on ADDRESS: " 
		<< inet_ntoa(serverSocketAddress.sin_addr) // convert the IP address (binary) in string
		<< " PORT: " << ntohs(serverSocketAddress.sin_port) // invert octets, beacause network have big endians before and we want to have little endians before
		<< " ***\n\n";
		ft_logger(ss.str(), INFO, __FILE__, __LINE__);

		if (currentSocket < 0)
		{
			std::ostringstream ss;
			ss << 
			"Server failed to setup connection on ADDRESS: " 
			<< inet_ntoa(serverSocketAddress.sin_addr) << "; PORT: " 
			<< ntohs(serverSocketAddress.sin_port);
			ft_logger(ss.str(), ERROR, __FILE__, __LINE__);
			throw std::runtime_error("Cannot setup connection");
		}

		// Update the maximum socket descriptor for select.
		if (currentSocket > maxSocket) {
			maxSocket = currentSocket;
		}
	}

	ft_logger("Server is listening on multiple ports...", INFO, __FILE__, __LINE__);

	//main loop
	recvSendLoop(serverSockets, maxSocket, servers);
}
