#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main() {
    // Create the socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n" << std::endl;
        return 1;
    }

    // Prepare the server address
    struct sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080); // Port 8080
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error binding socket to address" << std::endl;
        return 1;
    }

    // Listen for incoming connections 
    // To do : 
    // - we need to put that in a Queue
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Error listening for connections" << std::endl;
        return 1;
    }

    while (true) {
        // Accept a new connection
        // To do :
        // - accepts a network connection from the Queue one at a time
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        
        if (clientSocket < 0) {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }

        // Read the client's request
        // To do :
        // - reads the data (Request from the Queue) sent from the client over the network connection
        char buffer[1024];
        ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
        buffer[bytesRead] = '\0';

        // Server response
        // sends data (Response) to the client over the network connection.
        const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
        write(clientSocket, response, strlen(response));

        // Close the client connection
        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}