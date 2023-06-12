#include "http_tcpServer.h"
#include "utils.h"

int main() {
    // Create the socket
    TcpServer server = TcpServer("0.0.0.0", 8080);

	// parse_request("hello");
    while (true)
        server.acceptConnection();
    return 0;
}