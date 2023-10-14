// #include "http_tcpServer.hpp"
#include "utils.h"
#include "Request.hpp"
#include "parseConfig.hpp"

std::map<std::string, std::string>	cgi_types;
// Tconfig	temp_config = (Tconfig){"/workspaces/webserv", "/cgi-bin", "127.0.0.1", 8080, cgi_types};

int main(int ac, char **av) {
	(void)ac;
	(void)av;
	try
	{
		std::vector<Server>	servers;
		
		servers = parseConfig("./resources/default.conf");

		for (size_t i = 0; i < servers.size(); i++)
			std::cout << "Server " << i << ":\n" << servers[i] << "\n\n";
		
		handleConnections(servers);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
    return 0;
}
