#include "TcpServer.hpp"
#include "utils.h"
#include "Request.hpp"
#include "Service.hpp"

Tconfig	temp_config = (Tconfig){"/workspaces/webserv", "cgi-bin", "127.0.0.1", 8080};

int main(int ac, char **av) {
	srand((unsigned) time(NULL));
	
	if (ac > 2)
		std::cout << "Error: something go wrong about number arguments" << std::endl;
	if (ac == 2)
		Service webserv(av[1]);
	else{
		temp_config.host_port = 8080 + (rand() % 1000);
		TcpServer	server = TcpServer("0.0.0.0", temp_config.host_port);
		while (true)
        	server.runServer();
	} 
    return 0;
}