#include "utils.h"
#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>


std::vector<int> getPorts(std::vector<Server> &servers)
{
    std::vector<int> ports;

    for (int i = 0; i < servers.size(); i++)
    {
        ports.push_back(servers[i].getPort());
    }
    return ports;
}

handleConnections(std::vector<Server> &servers)
{
    const std::vector<int> portVec = getPorts(&servers);  // Get all the different ports in the servers

    // Create a vector to store socket descriptors for multiple ports.
    std::vector<int>    serverSockets;
    fd_set              tempSet;
    int                 maxSocket = 0;

    // Create and initialize sockets for each port.
    for (int i = 0; i < portVec.size(); ++i) {
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
        
        // Bind the socket to the address and port.
        if (bind(currentSocket,(struct sockaddr *)&serverSocketAddress, sizeof(serverSocketAddress)) < 0)
        {
            throw std::runtime_error("Cannot connect socket to address");
        }

        // Listen for incoming connections.
        

        // Update the maximum socket descriptor for select.
        

        if (serverSockets[i] > maxSocket) {
            maxSocket = serverSockets[i];
        }

    }
    std::cout << "Server is listening on multiple ports..." << std::endl;

    //main loop
    while (true)
    {
         // Add all server sockets to the set.
            // Loop through the server sockets to find the one that is ready.
                // Accept and handle the connection on the port i.
                // Handle the connection on this port as needed.
    }

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