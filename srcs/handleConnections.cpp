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

#include <sys/types.h>
#include <sys/wait.h>

#include "utils.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "CGI.hpp"

#include <ctime>

#define NOTSET -2

struct CgiState {
	std::string	readBuffer;
	std::string	writeBuffer;
	int			readFd;
	int			writeFd;
	int			clientSocket;
	std::clock_t start;
};

struct ClientState {
	std::string	incompleteRequest; // buffer for storing partial HTTP request
	std::string	incompleteResponse; // buffer for storing partial HTTP response
	bool		isCGI;
	bool		isChunked;
	bool		isReceivingBody;
	bool		isClosing;
	bool		isClosed;
	bool		requestCompleted;
	int			cgi_pid;
	size_t		contentLength;
	size_t		receivedLength;
	std::map<std::string, std::string>	header;
	std::string							body;
	std::string							info;
	std::queue<std::string>				responseQueue;
	// ... other states like request headers, method, etc.
};

void	ft_close(int &fd)
{
	if (fd != NOTSET)
		close(fd);
	fd = NOTSET;
}

void    parseHttpRequest(ClientState &client)
{
	size_t pos = client.incompleteRequest.find("\r\n\r\n"); // <-- seach for header-body seperator
	if (!client.isReceivingBody && pos != std::string::npos) // <-- if client isnt receiving body and seperator is found
	{
		if (client.incompleteRequest.size() >= 6)
		{
			if (client.incompleteRequest.substr(0, 3) != "GET"
			&& client.incompleteRequest.substr(0, 4) != "POST"
			&& client.incompleteRequest.substr(0, 6) != "DELETE")
			{
				ft_logger("Error: invalid request", ERROR, __FILE__, __LINE__);
				ft_logger("Request: " + client.incompleteRequest, DEBUG, __FILE__, __LINE__);
				throw 400;
			}
		}
		std::istringstream  iss(client.incompleteRequest.substr(0, pos + 4)); // <-- put header part of the request into istream
		std::string         key, val;

		std::getline(iss, client.info); // <-- put request first line into info to be passed later
		client.info = trimWhiteSpace(client.info);
		// parse header into map of key: values
		while(std::getline(std::getline(iss, key, ':') >> std::ws, val))
			client.header[trimWhiteSpace(toLowercase(key))] = trimWhiteSpace(val.substr(0, val.size() - 1));

		client.incompleteRequest.erase(0, pos + 4); // <-- erase what has been parsed

		// if Content-Length is present in header setup values to start receiving the body
		if (client.header["content-length"].empty())
			client.requestCompleted = true;
		else
		{
			client.contentLength = atoi(client.header["content-length"].c_str());
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
	ft_logger(ss.str(), DEBUG, __FILE__, __LINE__);
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
	ft_logger(msg, INFO, __FILE__, __LINE__);
	global_running_flag = 0; // Set the flag to break out of the loop
}

void	recvSendLoop(std::vector<int> &serverSockets, int &maxSocket, std::vector<Server> &servers)
{
	// Create a set to watch for activity on client sockets.
	fd_set  readSet, writeSet;
	// Create a map to store the state of each client socket.
	std::map<int, ClientState>				clients;
	std::map<int, ClientState>::iterator	it;
	// Create a map to store the state of each cgi pipes.
	std::map<int, CgiState>					cgi_map;
	std::map<int, CgiState>::iterator		cgi_it;
	
	// Request request;
	// Response response;

	// Create a timeval struct for the timeout.
	struct timeval timeout;
	timeout.tv_sec = 10; // <-- seconds
	timeout.tv_usec = 0; // <-- microseconds

	signal(SIGINT, signalHandler); // <-- set signal handler for SIGINT so program can exit gracefully

	while (global_running_flag) // <-- loop while global_running_flag is true (set to false by signal handler)
	{
		// Reset the read and write sets
		ssize_t bytesReceived = 0;
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);

		for (size_t i = 0; i < serverSockets.size(); )
		{
			if (fcntl(serverSockets[i], F_GETFD) == 0)
			{
				FD_SET(serverSockets[i], &readSet);
				i++; // Only increment if we didn't erase an element
			}
			else
			{
				ft_close(serverSockets[i]);
				serverSockets.erase(serverSockets.begin() + i);
				ft_logger("Error: fcntl() failed", ERROR, __FILE__, __LINE__);
			}
		}

		for (cgi_it = cgi_map.begin(); cgi_it != cgi_map.end(); )
		{
			// int			pid = cgi_it->first;
			CgiState&	cgi = cgi_it->second;
			if (cgi.readFd == NOTSET && cgi.writeFd == NOTSET)
			{
				cgi_map.erase(cgi_it++);
				ft_logger("CGI read and write pipes not set", DEBUG, __FILE__, __LINE__);
			}
			else
			{
				if (fcntl(cgi.readFd, F_GETFD) == 0)
					FD_SET(cgi.readFd, &readSet);
				else
					ft_logger("Error: cgi read pipe broken or closed", ERROR, __FILE__, __LINE__);
				if (fcntl(cgi.writeFd, F_GETFD) == 0)
					FD_SET(cgi.writeFd, &writeSet);
				else
					ft_logger("Error: cgi write pipe broken or closed", ERROR, __FILE__, __LINE__);
				cgi_it++;
			}
		}

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
				ft_close(clientSocket);
				clients.erase(it++);
			}
			else if (fcntl(clientSocket, F_GETFD) == -1) // <-- if client is broken or closed then erase from clients
			{
				// ft_close(clientSocket);
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
					// Invalid file descriptor
					perror("fcntl write");
				}
			}
		}

		// wait for activity on sockets in readSet and writeSet
		int activity = select(maxSocket + 1, &readSet, &writeSet, NULL, NULL);
		if (activity == -1)
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
		else if (activity == 0) // handle timeout
		{
			ft_logger("Timeout occurred: No data after " + SSTR(timeout.tv_sec) + " seconds", INFO, __FILE__, __LINE__);
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

				clients[clientSocket] = (ClientState){std::string(), std::string(), false, false, false, false, false, false, NOTSET, 0, 0, 
													  std::map<std::string, std::string>(), std::string(), std::string(), std::queue<std::string>()};

				std::stringstream ss;
				ss	<< "New connection incomming: " 
					<< inet_ntoa(clientSocketAddress.sin_addr) 
					<< ":" << ntohs(clientSocketAddress.sin_port);
				ft_logger(ss.str(), INFO, __FILE__, __LINE__);
			}
		}

		// Loop through cgi_map to check for readiness for reading and writing
		for (cgi_it = cgi_map.begin(); cgi_it != cgi_map.end(); cgi_it++)
		{
			// int			pid = cgi_it->first;
			int&		cgiOut = cgi_it->second.readFd;
			int&		cgiIn = cgi_it->second.writeFd;
			CgiState&	cgi = cgi_it->second;

			if (FD_ISSET(cgiOut, &readSet))
			{
				char	buffer[1024];

				bytesReceived = read(cgiOut, buffer, sizeof(buffer));
				if (bytesReceived < 0)
				{
					ft_logger("Error reading from CGI", ERROR, __FILE__, __LINE__);
					ft_close(cgiOut);
					// cgi_read_map.erase(cgi_read_it++);
					// continue ;
				}
				else if (bytesReceived == 0)
				{
					ft_logger("CGI pipe read returned zero", INFO, __FILE__, __LINE__);
					ft_close(cgiOut);
					if (!cgi.readBuffer.empty())
						clients[cgi.clientSocket].responseQueue.push(cgi.readBuffer); // DANGER because cgi.clientSocket might not exist
					else
					{
						ft_logger("Error: CGI returned empty response", ERROR, __FILE__, __LINE__);
						clients[cgi.clientSocket].responseQueue.push(makeResponse(500, "CGI DID NOT RESPOND")); // push 500 to responseQueue
					}
					cgi.readBuffer.clear();
					// cgi_read_map.erase(cgi_read_it++);
					// continue ;
				}
				else
				{
					ft_logger("Reading from cgi..", INFO, __FILE__, __LINE__);
					cgi.readBuffer.append(buffer, bytesReceived); // <-- append received data
					ft_logger("RESPONSE FROM CGI: " + cgi.readBuffer, DEBUG, __FILE__, __LINE__);
					// clients[cgi.clientSocket].responseQueue.push(makeResponse(500, ""));
					// if count is too high clear readbuffer and push 500 to responseQueue
				}
			}

			if (FD_ISSET(cgiIn, &writeSet) && cgi.writeBuffer.empty() == false) // <-- check if client is ready to write into
			{
				int bytesSent = write(cgiIn, cgi.writeBuffer.data(), cgi.writeBuffer.size());
				if(bytesSent < 0)
				{
					ft_logger("Error writing to CGI", ERROR, __FILE__, __LINE__);
					ft_close(cgiIn);
					// cgi_write_map.erase(cgi_write_it++);
					// continue ;
				}
				else if (bytesSent == 0)
				{
					ft_logger("CGI pipe sent zero bytes", INFO, __FILE__, __LINE__);
					ft_close(cgiIn);
					// cgi_write_map.erase(cgi_write_it++);
					// continue ;
				}
				else
				{
					ft_logger("TO CGI:\n" + cgi.writeBuffer.substr(0, bytesSent), DEBUG, __FILE__, __LINE__);
					cgi.writeBuffer.erase(0, bytesSent); // <-- erase sent data
				}
			}
		}

		// Loop through clients and check for readiness for reading and writing
		for (it = clients.begin(); it != clients.end(); it++)
		{
			// naming for clarity
			int         clientSocket = it->first;
			ClientState &client = it->second;

			if (client.cgi_pid != NOTSET)
			{
				CgiState	*cgi = cgi_map.find(client.cgi_pid) != cgi_map.end() ? &cgi_map[client.cgi_pid] : NULL;
				int status;
				pid_t result = waitpid(client.cgi_pid, &status, WNOHANG);
				if (result == 0)
				{
					// Process is still running
					if (cgi == NULL)
					{
						ft_logger("Error: cgi not found", ERROR, __FILE__, __LINE__);
					}
					else
					{
						std::clock_t now = std::clock();
						if (now - cgi->start > CLOCKS_PER_SEC * 10)
						{
							ft_logger("Error: CGI process timed out", ERROR, __FILE__, __LINE__);
							kill(client.cgi_pid, SIGKILL);
							ft_close(cgi->readFd);
							ft_close(cgi->writeFd);
							client.cgi_pid = NOTSET;
							client.responseQueue.push(makeResponse(500, "TIMEOUT"));
						}
						// else
							// ft_logger("CGI process still running", INFO, __FILE__, __LINE__);
					}
				}
				else if (result == client.cgi_pid)
				{
					// Process ended
					if (cgi == NULL)
					{
						ft_logger("Error: cgi not found", ERROR, __FILE__, __LINE__);
					}
					else
					{
						if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
							ft_logger("Error: CGI process exited with status " + SSTR(WEXITSTATUS(status)), ERROR, __FILE__, __LINE__);
						// close the cgi pipes
						ft_close(cgi->readFd);
						ft_close(cgi->writeFd);
						client.cgi_pid = NOTSET;
						ft_logger("CGI process ended", INFO, __FILE__, __LINE__);
					}
				}
				else
				{
					// An error occurred
					if (cgi == NULL)
					{
						ft_logger("Error: cgi not found", ERROR, __FILE__, __LINE__);
					}
					else
					{
						// close the cgi pipes
						ft_close(cgi->readFd);
						ft_close(cgi->writeFd);
						cgi->readFd = NOTSET;
						cgi->writeFd = NOTSET;
						client.cgi_pid = NOTSET;
						ft_logger("Error: waitpid() failed", ERROR, __FILE__, __LINE__);
					}
				}
			}
			
			else if (FD_ISSET(clientSocket, &readSet)) // <-- check if client is ready to read from
			{
				char    buffer[1024];
				bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
				if (bytesReceived < 0)
				{
					std::stringstream ss;
					ss << "Error reading from client: (" << clientSocket << ") " << std::endl;
					ft_logger(ss.str(), ERROR, __FILE__, __LINE__);
					ft_close(clientSocket);
					FD_CLR(clientSocket, &readSet);
					FD_CLR(clientSocket, &writeSet);
					client.isClosed = true;
					perror("bytes");
					continue ; // <-- NOT SURE IF THIS IS NEEDED
				}
				else if (bytesReceived == 0)
				{
					ft_logger("Client socket read returned zero", INFO, __FILE__, __LINE__);
					client.isClosing = true;
				}
				else
				{
					// std::string fullResponseStr;
					try
					{
						client.incompleteRequest.append(buffer, bytesReceived); // <-- append received data
						ft_logger("REQUEST:\n" + client.incompleteRequest, DEBUG, __FILE__, __LINE__);
						parseHttpRequest(client); // <-- parse the request into sdt::map client.header and std::string client.body
						if (client.requestCompleted == true) // <-- if the request has been fully parsed then create the response
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
								int read_fd = NOTSET, write_fd = NOTSET, cgi_pid = NOTSET;
								std::string fullResponseStr = response.processResponse(read_fd, write_fd, cgi_pid);
								ft_logger("RESPONSE STR:\n" + fullResponseStr, DEBUG, __FILE__, __LINE__);
								if (fullResponseStr.empty())
								{
									// CgiState cgi = {std::string(), client.body, read_fd, write_fd, clientSocket};
									cgi_map[cgi_pid] = (CgiState){std::string(), client.body, read_fd, write_fd, clientSocket, std::clock()};
									client.cgi_pid = cgi_pid;
								
									// update maxSocket as needed
									if (read_fd > maxSocket)
										maxSocket = read_fd;
									if (write_fd > maxSocket)
										maxSocket = write_fd;
								}
							
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
					}
					catch(int errorCode) // <-- if there was an error creating the cgi then send error response
					{
						Server s;
						Request r(s);
						Response response(errorCode, r, s);
						std::string data = response.jumpToErrorPage(errorCode);
						client.responseQueue.push(data);
					}
					catch(...) // <-- if there was an error creating the cgi then send error response
					{
						Server s;
						Request r(s);
						Response response(500, r, s);
						std::string data = response.jumpToErrorPage(500);
						client.responseQueue.push(data);
					}
				}
			}
			
			else if (FD_ISSET(clientSocket, &writeSet) && client.responseQueue.empty() == false) // <-- check if client is ready to write into
			{
				std::string	&responseStr = client.responseQueue.front();
				int bytesSent = send(clientSocket, responseStr.data(), responseStr.size(), 0);
				ft_logger("RESPONSE:\n" + responseStr, DEBUG, __FILE__, __LINE__);
				if(bytesSent < 0)
				{
					// Handle disconnect
					ft_close(clientSocket);
					FD_CLR(clientSocket, &readSet);
					FD_CLR(clientSocket, &writeSet);
					client.isClosed = true;
				}
				else if (bytesSent == 0)
				{
					// Handle disconnect
					ft_close(clientSocket);
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
						// ft_close(clientSocket);
						// client.isClosed = true;
					}
				}
			}
		}
	}

	/* Graceful Exit */
	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);
	// close all cgi pipe fds
	for (cgi_it = cgi_map.begin(); cgi_it != cgi_map.end(); cgi_it++)
	{
		if (cgi_it->second.readFd != NOTSET)
			ft_close(cgi_it->second.readFd);
		if (cgi_it->second.writeFd != NOTSET)
			ft_close(cgi_it->second.writeFd);
		// if waitpid is still running exit;
	}
	for (it = clients.begin(); it != clients.end(); it++)
		close(it->first);
	// close all server sockets
	for (size_t i = 0; i < serverSockets.size(); i++)
		ft_close(serverSockets[i]);
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
			ft_close(currentSocket);
			perror("bind failed"); // <-- COMMENT THIS OUT LATER
			ft_logger("Failed to bind socket: " + SSTR(htons(serverSocketAddress.sin_port)), ERROR, __FILE__, __LINE__);
			throw std::runtime_error("Cannot bind socket to address");
		}

		// Listen for incoming connections.
		if (listen(currentSocket, 10) < 0)
		{
			ft_close(currentSocket);
			ft_logger("Failed to listen on socket: " + SSTR(htons(serverSocketAddress.sin_port)), ERROR, __FILE__, __LINE__);
			throw std::runtime_error("Socket listen failed");
		}

		std::ostringstream ss;
		ss << "Listening on ADDRESS: " 
		<< inet_ntoa(serverSocketAddress.sin_addr) // convert the IP address (binary) in string
		<< " PORT: " << ntohs(serverSocketAddress.sin_port); // invert octets, beacause network have big endians before and we want to have little endians before
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
