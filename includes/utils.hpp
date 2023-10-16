#ifndef UTILS_HPP
# define UTILS_HPP
#include <iostream>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>
#include <map>
#include "Server.hpp"

#define EXAMPLE_RESPONSE "HTTP/1.1 200 OK \
\nDate: Mon, 27 Jul 2009 12:28:53 GMT \
\nServer: Apache/2.2.14 (Win32) \
\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT \
\nContent-Length: 88 \
\nContent-Type: text/html \
\nConnection: Closed \
\n\n \
\n<html> \
\n<body> \
\n<h1>Hello, World!</h1> \
\n</body> \
\n</html>"

#define EXAMPLE_REQUEST "GET / HTTP/1.1 \
\nHost: 127.0.0.1:8080 \
\nConnection: keep-alive \
\nsec-ch-ua: \"Not.A/Brand\";v=\"8\", \"Chromium\";v=\"114\" \
\nsec-ch-ua-mobile: ?0 \
\nsec-ch-ua-platform: \"macOS\" \
\nDNT: 1 \
\nUpgrade-Insecure-Requests: 1 \
\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36 \
\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7 \
\nSec-Fetch-Site: none \
\nSec-Fetch-Mode: navigate \
\nSec-Fetch-User: ?1 \
\nSec-Fetch-Dest: document \
\nAccept-Encoding: gzip, deflate, br \
\nAccept-Language: en-US,en;q=0.9"

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#define ERROR		-1
#define SUCCESS		0
#define BUFFER_SIZE	1024

typedef enum e_method
{
	GET,
	POST,
	DELETE,
	UNDEFINED
} t_method;

// typedef struct TempConfig
// {
// 	std::string	root;
// 	std::string	cgi_folder;
// 	std::string	name;
// 	int			host_port;
// 	std::map<std::string, std::string> cgi_types;
// } Tconfig;

// extern Tconfig	temp_config;

bool						pathExists(std::string path);
std::string					readFile(std::string location);
std::vector<std::string>	splitUrl(std::string url);
bool						pathIsDir(std::string path);
bool						isNumeric(std::string const &str);

void	handleConnections(std::vector<Server> &servers);

#endif