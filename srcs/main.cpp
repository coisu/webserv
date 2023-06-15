#include "http_tcpServer.h"
#include "utils.h"
#include "Request.hpp"

int main(int ac, char **av) {
	// Create the socket
	// parse_request(EXAMPLE_INPUT);
	int	port = 8081;
	if (ac > 1)
		port = atoi(av[1]);
	// Request		get_method(EXAMPLE_INPUT);
	// get_method.printHead();
	TcpServer	server = TcpServer("0.0.0.0", port);

    while (true)
        server.runServer();
    return 0;
}