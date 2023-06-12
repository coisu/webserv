#include "http_tcpServer.h"
#include "utils.h"
#include "Method.hpp"
#include "Get.hpp"

int main() {
	// Create the socket
	// parse_request(EXAMPLE_INPUT);
	Get		get_method(EXAMPLE_INPUT);
	get_method.printHead();
	TcpServer	server = TcpServer("0.0.0.0", 8080);

	while (true)
		server.acceptConnection();
	return 0;
}