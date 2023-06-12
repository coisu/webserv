#include "../includes/http_tcpServer.h"

int main() {
    // Create the socket
    TcpServer server = TcpServer("0.0.0.0", 8080);

    while (true)
        server.runServer();
    return 0;
}