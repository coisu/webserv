#include "http_tcpServer.h"
#include "utils.h"
#include "Request.hpp"

Tconfig	temp_config = (Tconfig){"/workspaces/webserv", "cgi-bin", "127.0.0.1", 8080};

int main(int ac, char **av) {
	srand((unsigned) time(NULL));
	temp_config.host_port = 8080 + (rand() % 1000);
	if (ac > 1)
		temp_config.host_port = atoi(av[1]);
	TcpServer	server = TcpServer("0.0.0.0", temp_config.host_port);

    while (true)
        server.runServer();
    return 0;
}