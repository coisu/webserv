#include "utils.hpp"
#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

#define WRITEREADY true
#define READREADY false

void    recvSendLoop(std::vector<int> &serverSockets, int &maxSocket)
{
    fd_set  readSet, writeSet;
    std::map<int, bool>             clientSockets;
    std::map<int, bool>::iterator   it;
    std::map<int, std::string>      clientMsg;
    // std::map<int, std::string>::iterator   it;
    char	                        buffer[1024];

    while (true)
    {
        ssize_t bytesReceived = 0;
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);

        // Add all server sockets to the set.
        for (size_t i = 0; i < serverSockets.size(); i++)
            FD_SET(serverSockets[i], &readSet);
        
        // Add client sockets to the readSet or writeSet
        for (it = clientSockets.begin(); it != clientSockets.end(); it++)
        {
            if (it->second == WRITEREADY)
                FD_SET(it->first, &writeSet);
            else if (it->second == READREADY)
                FD_SET(it->first, &readSet);
        }

        if (select(maxSocket + 1, &readSet, &writeSet, NULL, NULL) == -1)
        {
            throw std::runtime_error("Select() failed");
            // continue ;
        }
        // Loop through the server sockets to find the one that is ready.
        for (size_t i = 0; i < serverSockets.size(); i++)
        {
            // check each server socket to see if its ready
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
                clientSockets[clientSocket] = READREADY;
                // add client socket to readSet
                // FD_SET(clientSocket, &readSet);
                std::cout << "New connexion comming: " 
                            << inet_ntoa(clientSocketAddress.sin_addr) 
                            << ":" << ntohs(clientSocketAddress.sin_port) 
                            << std::endl;
            }
            for (it = clientSockets.begin(); it != clientSockets.end(); it++)
            {
                // check for data on client socket
                if (FD_ISSET(it->first, &readSet))
                {
                    bytesReceived = recv(it->first, buffer, sizeof(buffer), 0);
                    clientMsg[it->first].append(buffer, bytesReceived);
                    std::cout << "\n----MESSGE----\n" << clientMsg[it->first] << "\n-----END-----\n";
                    if (bytesReceived > 0)
                        std::cout << "\n----BUFFER----\n" << buffer << "\n-----END-----\n";
                    else if (bytesReceived == 0)
                        std::cout << "\n-NO BUFFER RECEIVED-\n";
                    else
                        std::cout << "\n-ERROR RECEIVING FROM SOCKET-\n";
                    //recv
                    // if recv finished reading set client to WRITEREADY in the clientSocket map
                    // recv -> parse request -> launch response
                }
                else if (FD_ISSET(it->first, &writeSet))
                {
                    //send
                    // if recv finished reading set client to WRITEREADY in the clientSocket map
                }
                // Handle the connection on this port as needed.
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

        // // Set to non-blocking
        // if (fcntl(currentSocket, F_SETFL, O_NONBLOCK) < 0)
        //     throw std::runtime_error("Socket set non-blocking failed");

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