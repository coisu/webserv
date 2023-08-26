#include "http_tcpServer.h"
#include "utils.h"
#include "Request.hpp"
#include "parseConfig.hpp"

std::map<std::string, std::string>	cgi_types;
Tconfig	temp_config = (Tconfig){"/workspaces/webserv", "/cgi-bin", "127.0.0.1", 8080, cgi_types};

int main(int ac, char **av) {
	temp_config.cgi_types[".sh"] = "/bin/bash", temp_config.cgi_types[".py"] = "/usr/bin/python3";
	
	srand((unsigned) time(NULL));
	temp_config.host_port = 8080 + (rand() % 1000);
	if (ac > 1)
		temp_config.host_port = atoi(av[1]);
	TcpServer	server = TcpServer("0.0.0.0", temp_config.host_port);
	// ParseConfig	config("./resources/default.conf");
	parseConfig("./resources/default.conf");
    while (true)
        server.runServer();
    return 0;
}
