

// char **env;
// char *url;
// char *proto;


// -> read request (from fd 0 to max fd)
// -> parse Request & create vector(structure) for server as much as number of server

// 	server can be more then one in config file.
// 	probably, in the struct or class of Parser,
// 	it has string array or vector<string> for the whole contents of each server config,
// 	and also std::vector<configServer> and int(size_t) nb_server value
// 	* based on the each string splited in vector<string>, set the each vector<configServer> value

// 	And class configServer will has index, port, host, server_name, server_addr(sockaddr_in), max_body_client, listen_fd, maybe map for error and etc
// 									+ struct or vector<string> for Location

// -> check is there duplicate server in the vector<configServer>
// -> create socket with server_address and server setting, (socket, setsockopt, bind)
// -> main loop start!!
// 	> select 
// 	goes through all file descriptors from 0 till the biggest fd in the set.

// 	check file descriptors returend from select():
//  	* if server fd --> accept new client
//  	* if client fd in read_set --> read message from client
//  	* if client fd in set_set:
//  	* 1- If it's a CGI response and Body still not sent to CGI child process
// 		--> Send request body to CGI child process.
//  	* 2- If it's a CGI response and Body was sent to CGI child process
// 		--> Read outupt from CGI child process.
//  	* 3- If it's a normal response
// 		--> Send response to client.

// -> accept connections,
// -> read request fd one by one,(if it has problem remove it from the set and close socket)
// -> read sucessfully, update last message time.
// -> assign server to the client
// -> start to build response
// 	-> pipe and fork, excute CGI
// 	-> Method GET-read, POST-set, DELETE-delete
// 	-> build body, html index
// 	-> append status line amd header contents 

// -> check is the host name, port and servername
// -> send the response, with send function, to client socket fd
// 	when it is too long, send it divided in several times
// 	And when send or wirte function returns 0 or -1(error throw),
// 	remove Connection(Client)

// GET :   read the resource, make a respond
//         check MIME type
//         check Last-Modified Time, add in the header and return
//         When the resource is directory, search the index file and read, or directory listening
//         * when it listen the directory, it needed to test with deep sub-directory
//             it supposed to listen everytime goes deeper to the sub-directory

// POST :  except the case(body size == 0), basically same with GET.
//         so it can be redirection with Method excution function
//         for this project, if the resource is not CGI,
//         then response will not be changed either body exist or not, and query exist or not.

// DELETE : delete resource that pointed by url
//          return in response body, info which file is deleted
//          *unsafety Method


// The server will put a html file to the body of response,
// and then send it back to client when client aske for one

// GET /hello.htm HTTP/1.1
// User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
// Host: 127.0.0.1:8080
// Accept-Language: en-us
// Accept-Encoding: gzip, deflate
// Connection: Keep-Alive

// GET /index HTTP/1.1
// Host: 127.0.0.1:8080
// Connection: keep-alive
// Accept: */*
// User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.22 (KHTML, like Gecko)     Chrome/25.0.1364.97 Safari/537.22
// Accept-Encoding: gzip,deflate,sdch
// Accept-Language: en-US,en;q=0.8
// Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3



// HTTP/1.1 200 OK
// Date: Mon, 23 May 2005 22:38:34 GMT
// Content-Type: application/json; charset=UTF-8
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





// NEEDED OR NOT, CHUNK BODY PARSER?!!

#include "Response.hpp"

Response::Response()
{
	path = "";
	location = "";
	body = "";
	body_len = 0;
	auto_index = false;
	code = 0;
	response_content = "";
}

Response::Response(Request &request) : request(request)
{
	path = "";
	location = "";
	body = "";
	body_len = 0;
	auto_index = false;
	code = request.code;
	response_content = "";
}

Response::~Response() {}

void	Response::buildResponse()
{
	Response::setHeader();
	Response::setBody();
}

void	Response::setHeader()
{
	Response::setStatusLine();
	Response::setDate();
	Response::setContentType();
	Response::setContentLength();
	Response::setServer();
	Response::setConnection();
	Response::setLocation();
}

void	Response::setBody()
{

}

void	Response::setStatusLine()
{
	std::stringstream ss;
	std::string strCode;

	ss << code;
	ss >> strCode;

	response_content.append("HTTP/1.1 ");
	response_content.append(strCode + " ");
	response_content.append(statusCode[code]);
	response_content.append("\r\n");
}

// Date: Mon, 23 May 2005 22:38:34 GMT
void	Response::setDate()
{
	std::string date;
	time_t cur = time(0);
	struct tm *gmt = gmtime(&cur);
	strftime(const_cast<char*>(date.c_str()), sizeof(date), "Date: %a, %d %b %Y %H:%M:%S %Z", gmt);
	response_content.append(date);
	response_content.append("\r\n");
	
}

// Content-Type: application/json; charset=UTF-8
void	Response::setContentType()
{
	response_content.append("Content-Type: ");
	int extnPos = path.rfind(".", std::string::npos);

	if(extnPos != std::string::npos && e_code == 200)
        response_content.append(mimeList.getMimeType(path.substr(extnPos)));
    else
    	response_content.append(mimeList.getMimeType("default"));
    response_content.append("\r\n");
}

// Content-Length: 155
void	Response::setContentLength()
{
	std::stringstream ss;
	std::string	body_len;

	ss << body.length();
	ss >> body_len;

	response_content.append("Content-Length: ");
	response_content.append(body_len);
	response_content.append("\r\n");
}

// Server: BestServ (Unix) (Red-Hat/Linux)
void	Response::setServer()
{
	response_content.append("Server: nginx 1.0.15\r\n");
}

// Connection: close / keep-alive
void	Response::setConnection()
{
    if(request.parse_request[Connection] == "keep-alive")
        response_content.append("Connection: keep-alive\r\n");
	else
		response_content.append("Connection: colse\r\n");
}





// void http::Server::_handleEpollout(Socket &sock, std::pair< http::Request, t_serverData > &data,
//                                    struct epoll_event *event, int epoll_fd)
// {
//     http::Response response;
//     t_serverData serverData = _getServerData(sock, data.first.header("host"));

//     if (data.first.isBodyTooLarge())
//         response = http::Response(http::PAYLOAD_TOO_LARGE);
//     else
//     {
//         try
//         {
//             response = handleRequest(data.first, serverData);
//         }
//         catch (cgi::CGIException &e)
//         {
//             throw Socket::SocketException();
//         }
//     }

//     // _log(data.first, response);
//     sock.send(response.toString(data.second.error_page));
//     if (!data.first.keepAlive() || data.first.isBodyTooLarge())
//         _removeAcceptedFD(sock);
//     else
//     {
//         data.first.clear();
//         event->events = EPOLLIN;
//         epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sock.Fd(), event);
//     }
// }