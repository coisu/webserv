#ifndef UTILS_H
# define UTILS_H
#include <iostream>

#define EXAMPLE_INPUT "GET / HTTP/1.1 \
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

typedef enum e_method
{
	GET,
	POST,
	DELETE,
	UNDEFINED
} t_method;

e_method	getMethodType(std::string info);
std::string	getURL(std::string info);

#endif