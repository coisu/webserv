// HTTP/1.1 200 OK
// Date: Mon, 23 May 2005 22:38:34 GMT
// Content-Type: text/html; charset=UTF-8
// Content-Length: 155
// Last-Modified: Wed, 08 Jan 2003 23:11:55 GMT
// Server: BestServ (Unix) (Red-Hat/Linux)
// ETag: "3f80f-1b6-3e1cb03b"
// Accept-Ranges: bytes
// Connection: close

// <html>
// <head>
//     <title>An Example Page</title>
// </head>
// <body>
//     <p>Exemple of a server response.</p>
// </body>
// </html>

// char **env;
// char *url;
// char *proto;


-> read request (from fd 0 to max fd)
-> parse Request & create vector(structure) for server as much as number of server

	server can be more then one in config file.
	probably, in the struct or class of Parser,
	it has string array or vector<string> for the whole contents of each server config,
	and also std::vector<configServer> and int(size_t) nb_server value
	* based on the each string splited in vector<string>, set the each vector<configServer> value

	And class configServer will has index, port, host, server_name, server_addr(sockaddr_in), max_body_client, listen_fd, maybe map for error and etc
									+ struct or vector<string> for Location

-> check is there duplicate server in the vector<configServer>
-> create socket with server_address and server setting, (socket, setsockopt, bind)
-> main loop start!!
	> select 
	goes through all file descriptors from 0 till the biggest fd in the set.

	check file descriptors returend from select():
 	* if server fd --> accept new client
 	* if client fd in read_set --> read message from client
 	* if client fd in write_set:
 	* 1- If it's a CGI response and Body still not sent to CGI child process
		--> Send request body to CGI child process.
 	* 2- If it's a CGI response and Body was sent to CGI child process
		--> Read outupt from CGI child process.
 	* 3- If it's a normal response
		--> Send response to client.

-> accept connections,
-> 
-> parsing done successfully, check is the host name, port and servername
-> pipe & fork before running CGI(need to be excute in child process)
-> excute CGI with the env
-> parse the result of CGI again
-> create Response header and body
-> convert every hexadecimal token to decimal(string)
-> send the response, with send function, to client socket fd
	when it is too long, send it divided in several times
	And when send or wirte function returns 0 or -1(error throw),
	remove Connection(Client)

GET :   read the resource, make a respond
        check MIME type
        check Last-Modified Time, add in the header and return
        When the resourceis directory, search the index file and read, or directory listening
        * when it listen the directory, it needed to test with deep sub-directory
            it supposed to listen everytime goes deeper to the sub-directory

POST :  except the case(body size == 0), basically same with GET.
        so it can be redirection with Method excution function
        for this project, if the resource is not CGI,
        then response will not be changed either body exist or not, and query exist or not.

DELETE : delete resource that pointed by url
         return in response body, info which file is deleted
         *unsafety Method


std::string 


// headers.push_back(getDateHeader());
// headers.push_back(getServerHeader(this));

// if (status == CGI_SUCCESS_CODE)
// 	createCGIResponse(status, headers, body);
// if (status >= 400 && status <= 599)
// {
// 	if (!location.get_m_error_page().empty())
// 		body = location.get_m_error_page();
// 	else
// 		body = m_default_error_page;
// }
// if (!ft::hasKey(ft::stringVectorToMap(headers), "Transfer-Encoding"))
// 	headers.push_back("Content-Length:" + ft::to_string(body.size()));
// if (!body.empty())
// 		headers.push_back("Content-Language:ko-KR");
// if (status / 100 != 2)
// 	headers.push_back("Connection:close");
// if (status / 100 == 3 && !ft::hasKey(ft::stringVectorToMap(headers), "Location"))
// 	headers.push_back("Location:/");
// if (status == 504 && !ft::hasKey(ft::stringVectorToMap(headers), "Retry-After"))
// 	headers.push_back("Retry-After:3600");
// if (connection.get_m_request().get_m_method() == Request::HEAD)
// 	body.clear();