#include "http_tcpServer.h"
#include "utils.h"
#include "Request.hpp"

Tconfig	temp_config = (Tconfig){"/workspaces/webserv", "cgi-bin"};

int main(int ac, char **av) {
	srand((unsigned) time(NULL));
	int port = 8080 + (rand() % 1000);
	if (ac > 1)
		port = atoi(av[1]);
	TcpServer	server = TcpServer("0.0.0.0", port);

    while (true)
        server.runServer();
    return 0;
}