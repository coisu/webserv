#include "http_tcpServer.h"

int main() {
    // Create the socket
    TcpServer server = TcpServer("0.0.0.0", 8080);
    //int serverSocket = socket(AF_INET, SOCK_STREAM, 0);


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