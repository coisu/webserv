#include "utils.hpp"
#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>

#define WRITEREADY true
#define READREADY false

#include <fstream>
#include <sstream>

struct ClientState {
    std::string incompleteRequest; // buffer for storing partial HTTP request
    std::string incompleteResponse; // buffer for storing partial HTTP response
    bool    isChunked;
    bool    isReceivingBody;
    size_t  contentLength;
    size_t  receivedLength;
    std::map<std::string, std::string>  header;
    std::string                         body;
    std::string                         info;
    // ... other states like request headers, method, etc.
};

void    parseHttpRequest(ClientState &client)
{
    size_t pos = client.incompleteRequest.find("\r\n\r\n"); // <-- seach for header-body seperator
    if (!client.isReceivingBody && pos != std::string::npos) // <-- if client isnt receiving body and seperator is found
    {
        std::istringstream  iss(client.incompleteRequest.substr(0, pos + 4)); // <-- put header part of the request into istream
        std::string         key, val;

        std::getline(iss, client.info); // <-- put request first line into info to be passed later
        // parse header into map of key: values
        while(std::getline(std::getline(iss, key, ':') >> std::ws, val))
            client.header[key] = val.substr(0, val.size() - 1);

        client.incompleteRequest.erase(0, pos + 4); // <-- erase what has been parsed

        // if Content-Length is present in header setup values to start receiving the body
        if (!client.header["Content-Length"].empty())
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
        client.isReceivingBody = false;
}

std::string    printClient(ClientState &client)
{
    std::ostringstream os;
    std::map<std::string, std::string>::iterator it;
    os << "INFO:" << std::endl << client.info  << ";" << std::endl;
    os << "HEADER:" << std::endl;
    for (it = client.header.begin(); it != client.header.end(); it++)
    {
        os << it->first << ": " << it->second << ";" << std::endl;
    }
    os << "BODY:" << std::endl;
    os << client.body << ";" << std::endl;
    // std::cout << os.str();
    return (os.str());
}

std::string makeResponse(int code, std::string body)
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << code << " OK\r\n"
          << "Content-Type: text/html\r\n" //text/plain\r\n"
          << "Content-Length: " << body.size() << "\r\n"
          << "\n" << body << "\r\n";
    // std::cout << ss.str();
    return (ss.str());
}

void    recvSendLoop(std::vector<int> &serverSockets, int &maxSocket)
{
    fd_set  readSet, writeSet;
    std::map<int, ClientState> clients;
    std::map<int, ClientState>::iterator   it;

    while (true)
    {
        ssize_t bytesReceived = 0;
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);

        // Add all server sockets to the set.
        for (size_t i = 0; i < serverSockets.size(); i++)
            FD_SET(serverSockets[i], &readSet);

        // Add all client sockets to writeSet and readSet
        for (it = clients.begin(); it != clients.end(); it++)
        {
            FD_SET(it->first, &readSet);
            FD_SET(it->first, &writeSet);
        }

        if (select(maxSocket + 1, &readSet, &writeSet, NULL, NULL) == -1)
        {
            throw std::runtime_error("Select() failed");
            // continue ;
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
                clients[clientSocket] = (ClientState){std::string(), std::string(), false, false, 0, 0, std::map<std::string, std::string>(), std::string(), std::string()};
                // clients[clientSocket] = (ClientState){0, 0, 0, 0, 0, 0, std::map<std::string, std::string>(), 0, 0};
                std::cout << "New connexion comming: " 
                            << inet_ntoa(clientSocketAddress.sin_addr) 
                            << ":" << ntohs(clientSocketAddress.sin_port) 
                            << std::endl;
            }
        }
        // Loop through clients and check for readiness for reading and writing
        for (it = clients.begin(); it != clients.end(); it++)
        {
            // naming for clarity
            int         clientSocket = it->first;
            ClientState client = it->second;

            if (FD_ISSET(clientSocket, &readSet)) // <-- check if client is ready to read from
            {
                char    buffer[1024];
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                // std::cout << "\n----MESSGE----\n" << client.incompleteRequest << "\n-----END-----\n";
                if (bytesReceived <= 0)
                {
                    if (bytesReceived == -1)
                        throw std::runtime_error("Error reading from client");
                    if (bytesReceived == 0)
                    {
                        // std::cout << "RECV RETURNED ZERO\n";
                        // close socket
                    }
                }
                else
                {
                    client.incompleteRequest.append(buffer, bytesReceived); // <-- append received data
                    parseHttpRequest(client);
                    if (bytesReceived < 1024)
                        client.incompleteResponse = makeResponse(200, printClient(client));
                }
            }
            if (FD_ISSET(clientSocket, &writeSet)) // <-- check if client is ready to write into
            {
                // std::cout << "READY TO WRITE\n";
                int bytesSent = send(clientSocket, client.incompleteResponse.data(), client.incompleteResponse.size(), 0);
                if(bytesSent <= 0) {
                    // Handle error or close
                } 
                else {
                    client.incompleteResponse.erase(0, bytesSent); // <-- erase sent data
                }
            }
        }
    }
    for (size_t i = 0; i < serverSockets.size(); i++)
        close(serverSockets[i]);
}

std::vector<int> getPorts(std::vector<Server> &servers)
{
    std::vector<int> ports;

    for (size_t i = 0; i < servers.size(); i++)
    {
        ports.push_back(servers[i].getPort());
    }
    return ports;
}

void handleConnections(std::vector<Server> &servers)
{
    const std::vector<int> portVec = getPorts(servers);  // Get all the different ports in the servers

    // Create a vector to store socket descriptors for multiple ports.
    std::vector<int>    serverSockets;
    int                 maxSocket = 0;

    // Create and initialize sockets for each port.
    for (size_t i = 0; i < portVec.size(); i++)
    {
        int currentSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (currentSocket == -1) {
            std::cerr << "Error creating socket for port " << i << std::endl;
        }
        serverSockets.push_back(currentSocket);

        // Set up the server address structure.
        struct sockaddr_in  serverSocketAddress;
        serverSocketAddress.sin_family = AF_INET; // for IPv4
        serverSocketAddress.sin_port = portVec[i];
        serverSocketAddress.sin_addr.s_addr = INADDR_ANY;

        std::cout << "PORT: " << ntohs(portVec[i]) << std::endl;
        
        // Bind the socket to the address and port.
        if (bind(currentSocket,(struct sockaddr *)&serverSocketAddress, sizeof(serverSocketAddress)) < 0)
        {
            throw std::runtime_error("Cannot connect socket to address");
        }

        // Listen for incoming connections.
        if (listen(currentSocket, 10) < 0)
            throw std::runtime_error("Socket listen failed");

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
    recvSendLoop(serverSockets, maxSocket);
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