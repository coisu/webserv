// #include "http_tcpServer.hpp"
#include "utils.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "parseConfig.hpp"

std::map<std::string, std::string>	cgi_types;
// Tconfig	temp_config = (Tconfig){"/workspaces/webserv", "/cgi-bin", "127.0.0.1", 8080, cgi_types};

int global_running_flag = 1;  // Set the flag to break out of the loop


int main(int ac, char **av) {
	(void)ac;
	(void)av;
	try
	{
		std::vector<Server>	servers;
		
		// Server s;
		// Request r(s);
		// Response Error(404, r, s);

		// ParseConfig	config("./resources/default.conf");
		if (ac == 2)
			servers = parseConfig(av[1]);
		else
			servers = parseConfig("./resources/default.conf");

		for (size_t i = 0; i < servers.size(); i++)
		{
			std::cout << "Server " << i << ":\n" << servers[i] << "\n\n";
			// servers[i].startServer();
		}
		
		handleConnections(servers);
		// while (true)
		// {
		// 	for (size_t i = 0; i < servers.size(); i++)
		// 	{
		// 		servers[i].startListen();
		// 		servers[i].runServer();
		// 	}
		// }
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
    return 0;
}
