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
		std::cerr << "Error: problem directing to server\n"; // return server error
	else
		std::cout << "Port Number: " << ntohs(sin.sin_port) << std::endl;

	// set the client values
	unsigned int clientPort = sin.sin_port;
	std::string clientName = client.header["Host"];
	clientName = clientName.substr(0, clientName.find(':'));

	// std::cout << "HOST: " << clientName
	// 		  << "\nclient PORT: " << sin.sin_port
	// 		  << "\nserver[0] PORT: " << servers[0].getPort() << std::endl;

	// // select default server (server with no name)
	// for (it = servers.begin(); it != servers.end(); it++)
	// {
	// 	if (it->getPort() == clientPort && it->getServerName().empty())
	// 	{
	// 		serverIndex = it - servers.begin();
	// 		break ;
	// 	}
	// }

	// The first server for a host:port will be the default for this host:port
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
			if (it->second.isClosed)
				clients.erase(it++);
			else if (it->second.isClosing && it->second.incompleteResponse.empty())
			{
				close(it->first);
				clients.erase(it++);
			}
			else
			{
				FD_SET(it->first, &readSet);
				// if (!it->second.incompleteResponse.empty()) // <-- only add to writeSet if there is something to send
					FD_SET(it->first, &writeSet);
				it++;
			}
		}

		// wait for activity on sockets in readSet and writeSet
		if (select(maxSocket + 1, &readSet, &writeSet, NULL, NULL) == -1)
		{
			// throw std::runtime_error("Select() failed");
			if (global_running_flag == true)
			{
				exit(1);
				perror("select"); // <-- COMMENT THIS OUT LATER
				std::cerr << "Error: select() failed\n";
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
					std::cerr << 
					"Server failed to accept incoming connection from clent ADDRESS: " 
					<< inet_ntoa(clientSocketAddress.sin_addr) << "; PORT: " 
					<< ntohs(clientSocketAddress.sin_port);
					continue ;
				}
				// update maxSocket as needed
				if (clientSocket > maxSocket)
					maxSocket = clientSocket;
				// clients[clientSocket] = (ClientState){};
				clients[clientSocket] = (ClientState){std::string(), std::string(), false, false, false, false, false, false, 0, 0, 
													  std::map<std::string, std::string>(), std::string(), std::string(), std::queue<std::string>()};
				// clients[clientSocket] = (ClientState){0, 0, 0, 0, 0, 0, std::map<std::string, std::string>(), 0, 0};
				std::cout << "New connection incomming: " 
							<< inet_ntoa(clientSocketAddress.sin_addr) 
							<< ":" << ntohs(clientSocketAddress.sin_port) 
							<< std::endl;
				ft_logger("New connection incomming", INFO, __FILE__, __LINE__);
			}
		}

		// Loop through cgi_map to check for readiness for reading
		for (cgi_it = cgi_map.begin(); cgi_it != cgi_map.end(); )
		{
			int	cgiSocket = cgi_it->first;
			CgiState &cgi = cgi_it->second;

			if (cgi.incompleteResponse.empty() && cgi.isFinished == true)
			{
				ft_logger("CGI is finished", INFO, __FILE__, __LINE__);
				close(cgiSocket);
				FD_CLR(cgiSocket, &readSet);
				cgi_map.erase(cgi_it++);
				continue ;
			}
			else if (FD_ISSET(cgiSocket, &readSet))
			{
				char	buffer[1024];
				bytesReceived = read(cgiSocket, buffer, sizeof(buffer));
				if (bytesReceived <= 0)
				{
					if (bytesReceived == -1)
					{
						ft_logger("CGI is finished", INFO, __FILE__, __LINE__);
						std::cerr << "Error reading from CGI" << std::endl;
						close(cgiSocket);
						FD_CLR(cgiSocket, &readSet);
						cgi.isFinished = true;
						cgi_map.erase(cgi_it++);
						continue ;
					}
					if (bytesReceived == 0)
					{
						ft_logger("CGI is finished", INFO, __FILE__, __LINE__);
						std::cout << "RECV CGI RETURNED ZERO\n";
						cgi.isFinished = true;
						clients[cgi.clientSocket].responseQueue.push(cgi.incompleteResponse);
						cgi.incompleteResponse.clear();
						cgi_map.erase(cgi_it++);
						continue ;
					}
				}
				else
				{
					ft_logger("CGI is finished", INFO, __FILE__, __LINE__);
					cgi.incompleteResponse.append(buffer, bytesReceived); // <-- append received data
					if (cgi.incompleteResponse.find("\r\n\r\n") != std::string::npos)
					{
						ft_logger("CGI is finished", INFO, __FILE__, __LINE__);
						std::cout << "CGI RESPONSE:\n" << cgi.incompleteResponse << std::endl;
						cgi.isFinished = true;
						clients[cgi.clientSocket].responseQueue.push(cgi.incompleteResponse);
						cgi.incompleteResponse.clear();
						close(cgiSocket);
						cgi_map.erase(cgi_it++);
						continue ;
					}
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
				if (bytesReceived <= 0)
				{
					if (bytesReceived == -1)
					{
						std::cerr << "Error reading from client" << std::endl;
						close(clientSocket);
						FD_CLR(clientSocket, &readSet);
						FD_CLR(clientSocket, &writeSet);
						client.isClosed = true;
						continue ;
					}
					if (bytesReceived == 0)
					{
						std::cout << "RECV RETURNED ZERO\n";
						client.isClosing = true;
					}
				}
				else
				{
					std::string fullResponseStr;
					client.incompleteRequest.append(buffer, bytesReceived); // <-- append received data
					parseHttpRequest(client); // <-- parse the request into sdt::map client.header and std::string client.body
					if (client.requestCompleted == true) // <-- if the request has been fully parsed then create the response
					{
						try
						{
							int idx = chooseServer(clientSocket, client, servers); // <-- select correct host according to hostname and server port
							if (idx < 0 || (size_t)idx >= servers.size())
								throw 404;
							Request request(client.header, client.body, client.info, servers[idx]); // <-- create request obj with ClientStatus info
							try
							{
								Response response(request, servers[idx]); // <-- create response with request obj and selected server
								std::pair<bool, Location> loc_pair = servers[idx].srchLocation(request.getLocPath());
								if (loc_pair.first && loc_pair.second.getIsCGI() 
								&& (!pathIsDir(request.getLocPath()) || !loc_pair.second.getIndex().empty()))
								{
									CGI cgi(servers[idx], loc_pair.second, request);
									// CGI cgi(servers[idx], request.getURL(), request.getMethodStr(), loc_pair.second.getCGIConfig());
									int fd;
									cgi.exec_cgi(fd);
									cgi_map[fd] = (CgiState){std::string(), false, clientSocket};
								}
								else
								fullResponseStr = response.processResponse();
								client.responseQueue.push(response.processResponse()); // <-- push processed response to the queue
								client.requestCompleted = false; // <-- set to false so that next message will be read
								// response.processResponse(statusCode);
							}
							catch(int errorCode)
							{
								std::cerr << "Error: " << errorCode << std::endl;
								// response.processError(errorCode);
								// return 404 error
							}
						}
						catch(int errorCode)
						{
							std::cerr << "Error: " << errorCode << std::endl;
							// return the error that corresponds to the error code
						}
						catch(std::exception &e)
						{
							std::cerr << "Error: " << e.what() << std::endl;
							// return server error 500
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
					// Handle error or close
					close(clientSocket);
					FD_CLR(clientSocket, &readSet);
					FD_CLR(clientSocket, &writeSet);
					client.isClosed = true;
				}
				else
				{
					responseStr.erase(0, bytesSent); // <-- erase sent data
					if (responseStr.empty())
						client.responseQueue.pop();
				}
			}
		}
	}
	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);
	for (it = clients.begin(); it != clients.end(); it++)
		close(it->first);
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
			// std::cerr << "Error creating socket for port " << i << std::endl;
			std::cerr << "Error creating socket for port " << port << std::endl;
		}
		serverSockets.push_back(currentSocket);

		// Set up the server address structure.
		struct sockaddr_in  serverSocketAddress;
		serverSocketAddress.sin_family = AF_INET; // for IPv4
		// serverSocketAddress.sin_port = portVec[i];
		serverSocketAddress.sin_port = port;
		serverSocketAddress.sin_addr.s_addr = INADDR_ANY;

		std::cout << "PORT: " << ntohs(port) << std::endl;
		
		// Bind the socket to the address and port.
		int yes = 1;
		if (setsockopt(currentSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
			perror("setsockopt"); // <-- COMMENT THIS OUT LATER
		}
		if (bind(currentSocket,(struct sockaddr *)&serverSocketAddress, sizeof(serverSocketAddress)) < 0)
		{
			close(currentSocket);
				perror("bind failed"); // <-- COMMENT THIS OUT LATER
			throw std::runtime_error("Cannot bind socket to address");
		}
		// Listen for incoming connections.
		if (listen(currentSocket, 10) < 0)
			{
				close(currentSocket);
			throw std::runtime_error("Socket listen failed");
			}
		std::ostringstream ss;
		ss << "\n*** Listening on ADDRESS: " 
		<< inet_ntoa(serverSocketAddress.sin_addr) // convert the IP address (binary) in string
		<< " PORT: " << ntohs(serverSocketAddress.sin_port) // invert octets, beacause network have big endians before and we want to have little endians before
		<< " ***\n\n";

		if (currentSocket < 0)
		{
			std::ostringstream ss;
			ss << 
			"Server failed to setup connection on ADDRESS: " 
			<< inet_ntoa(serverSocketAddress.sin_addr) << "; PORT: " 
			<< ntohs(serverSocketAddress.sin_port);
			throw std::runtime_error(ss.str());
		}

		// Update the maximum socket descriptor for select.
		if (currentSocket > maxSocket) {
			maxSocket = currentSocket;
		}
	}

	std::cout << "Server is listening on multiple ports..." << std::endl;

	//main loop
	recvSendLoop(serverSockets, maxSocket, servers);
}

// handleConnections(std::vector<Server> &servers)
// {
//     const int numPorts = getPorts(&servers);  // Change this to the number of ports you want to listen on.

//     // Create an array to store socket descriptors for multiple ports.
//     int serverSockets[numPorts];
//     fd_set readSet;
//     int maxSocket = 0;

//     // Create and initialize sockets for each port.
//     for (int i = 0; i < numPorts; ++i) {
//         serverSockets[i] = socket(AF_INET, SOCK_STREAM, 0);
//         if (serverSockets[i] == -1) {
//             std::cerr << "Error creating socket for port " << i << std::endl;
//             return -1;
//         }

//         // Set up the server address structure.
//         struct sockaddr_in serverAddress;
//         serverAddress.sin_family = AF_INET;
//         serverAddress.sin_port = htons(8080 + i); // Use different ports.
//         serverAddress.sin_addr.s_addr = INADDR_ANY;

//         // Bind the socket to the address and port.
//         if (bind(serverSockets[i], (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
//             std::cerr << "Error binding socket for port " << i << std::endl;
//             return -1;
//         }

//         // Listen for incoming connections.
//         if (listen(serverSockets[i], 5) == -1) {
//             std::cerr << "Error listening on port " << i << std::endl;
//             return -1;
//         }

//         // Update the maximum socket descriptor for select.
//         if (serverSockets[i] > maxSocket) {
//             maxSocket = serverSockets[i];
//         }
//     }

//     std::cout << "Server is listening on multiple ports..." << std::endl;

//     while (true) {
//         FD_ZERO(&readSet);

//         // Add all server sockets to the set.
//         for (int i = 0; i < numPorts; ++i) {
//             FD_SET(serverSockets[i], &readSet);
//         }

//         int readySockets = select(maxSocket + 1, &readSet, NULL, NULL, NULL);

//         if (readySockets > 0) {
//             // Loop through the server sockets to find the one that is ready.
//             for (int i = 0; i < numPorts; ++i) {
//                 if (FD_ISSET(serverSockets[i], &readSet)) {
//                     // Accept and handle the connection on the port i.
//                     struct sockaddr_in clientAddress;
//                     socklen_t clientAddressSize = sizeof(clientAddress);
//                     int clientSocket = accept(serverSockets[i], (struct sockaddr*)&clientAddress, &clientAddressSize);
//                     // Handle the connection on this port as needed.
//                 }
//             }
//         }
//     }

//     // Close sockets and perform cleanup when done.

//     return 0;

// }