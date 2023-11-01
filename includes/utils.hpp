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

#define IS_DIR 0
#define IS_REG 1
#define N_DIR 2
#define N_FOUND 3
#define UNDEFINE -1

#define READABLE 1<<2
#define WRITABLE 1<<1
#define EXCUTABLE 1

#ifndef LOG_LEVEL
# define LOG_LEVEL 1
#endif

#define reset "\e[m"                 //ANSI CODE 0   → resets all styles, it's the same of \e[0m

#define bold "\e[1m"                //# ANSI CODE 1   → increases intensity, with a slight color change
// #define rbold "\e[22m"              # ANSI CODE 22  → resets bold or dim (they are mutually exclusive styles)

#define dim "\e[2m"                 //# ANSI CODE 2   → decreases intensity, with a slight color change
// #define rdim "\e[22m"               # ANSI CODE 22  → resets bold or dim (they are mutually exclusive styles)

#define italic "\e[3m"              //# ANSI CODE 3   → italic
// #define ritalic "\e[23m"             # ANSI CODE 23  → resets italic

#define underline "\e[4m"           //# ANSI CODE 4   → underline
// #define runderline "\e[24m"         # ANSI CODE 24  → resets underline or doubleunderline (they are mutually exclusive styles)

#define doubleunderline "\e[21m"    //# ANSI CODE 21  → double underline (not supported by Konsole)
// #define rdoubleunderline "\e[24m"   # ANSI CODE 24  → resets underline or doubleunderline (they are mutually exclusive styles)

#define curlyunderline "\e[4:3m"    //# ANSI CODE 4:3 → curly underline (not supported by Konsole)
// #define rcurlyunderline "\e[4:0m"   # ANSI CODE 4:0 → resets curly underline

#define blink "\e[5m"               //# ANSI CODE 5   → blink
// #define rblink "\e[25m"             # ANSI CODE 25  → resets blink

// #define reverse "\e[7m"             //# ANSI CODE 7   → swaps text and background colors
// #define rreverse "\e[27m"           # ANSI CODE 27  → resets reverse

#define hidden "\e[8m"              //# ANSI CODE 8   → characters not displayed, helpful for passwords
// #define rhidden "\e[28m"            # ANSI CODE 28  → resets hidden

#define strikethrough "\e[9m"       //# ANSI CODE 9   → characters crossed by a central line
// #define rstrikethrough "\e[29m"     # ANSI CODE 29  → resets strikethrough

#define overline "\e[53m"           //# ANSI CODE 53 → overline
// #define roverline "\e[55m"          # ANSI CODE 55 → resets overline

#define yellowBg "\e[48;2;255;255;0m"

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

// #define ERROR		-1
// #define SUCCESS		0
#define BUFFER_SIZE	1024

extern int global_running_flag;

typedef enum e_method
{
	GET,
	POST,
	DELETE,
	INVALID
} t_method;

typedef enum e_level
{
	ERROR,
	INFO,
	DEBUG
} t_level;
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
int							pathIsDir(std::string path);
char						isPermit(std::string path);
bool						isNumeric(std::string const &str);
std::string					trimWhiteSpace( const std::string &str );

void	handleConnections(std::vector<Server> &servers);
void	ft_logger(std::string thing, int level, std::string FILE, int LINE);


#endif