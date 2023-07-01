

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
	redirect_code_ = 0;
	redirect_ = false;
	code = 0;
	response_content = "";
	header_size_ = 0;
}

Response::Response(Request &request) : request(request)
{
	path = "";
	location = "";
	body = "";
	body_len = 0;
	auto_index = false;
	redirect_code_ = 0;
	redirect_ = false;
	code = request.code;
	response_content = "";
	header_size_ = 0;
}

Response::~Response() {}

void	Response::buildResponse()
{
	Response::buildHeader();
	Response::buildBody();
}

void	Response::buildHeader()
{
	Response::setStatusLine();
	Response::setDate();
	Response::setContentType();
	Response::setContentLength();
	Response::setServer();
	Response::setConnection();
	Response::setLocation();
}

std::vector<Location>::iterator Response::matchLocation(std::string path, std::vector<Location> locs, std::string &matchedPath)
{
	int match_len = 0;

	std::vector<Location>::iterator it;
	std::vector<Location>::iterator ret;
	for (it = locs.begin(); it != locs.end(); it++)
	{
		std::string it_path = it->getPath();
		if (path.find(it_path) == 0)
		{
			if (it->it_path == "/" \
				|| path.length() == it_path.length() || path[it_path.length()] == '/')
			{
				if (it_path.length() > match_len)
				{
					matchedPath = it_path;
					match_len = it_path.length();
					ret = it;
				}
			}
		}
	}
	return ret;
}

int Response::checkAllowedMethods(Location &matchLoc)
{
	std::vector<int>methods = matchLoc.getAllowedMethods();
	if (request.getMethod() == GET && !methods.find(methods.begin(), methods.end(), GET)
		|| request.getMethod() == POST && !methods.find(methods.begin(), methods.end(), POST)
		|| request.getMethod() == DELETE && !methods.find(methods.begin(), methods.end(), DELETE))
		return (1);
	return  (0);
}

int	Response::checkMaxBody(Location &loc)
{
	if (request.getBody().length() > loc.getMaxBodySize())
		return (1);
	return (0);
}

// In response to a request with URI equal to this string,
// but without the trailing slash,
// a permanent redirect with the code 301 will be returned to the requested URI with the slash appended.
int	Response::checkReturn(Location &loc)
{
	if (!loc.getReturn().empty())
	{
		location = loc.getReturn();
		if (location.front() != '/')
			location.insert(0, "/");
		return (1);
	}
	return (0);
}

int	Response::checkCgi(std::string path_loc, Location &loc)
{
	std::string path_req = request.getPath();
	std::string extn;
	int	pos;

	if (path_req[0] && path_req.front() == '/')
		path_req = path_req.substr(1);

	std::string indexStr;
	if (path_req.back() != '/')
		indexStr = "/";
	indexStr.append(loc.getIndex())
	path_req.append(indexStr);

 file = data.root.substr(0, data.root.size() - 1) + request.header("Path") + loc._index;
 ret.setBodyCGI(cgi(request, loc, data, file).getOutput());
}

int	Response::isCgi(Location &loc)
{
	if (loc.getPath().find("cgi-bin") != std::string::npos)
		return (1);
	return (0);
}

int Response::checkLocation()
{
	std::string matchedPath;
	Location matchLoc = matchLocation(request.getPath(), server.getLocations());
	
	if (matchedPath.length())
	{
		if (checkAllowedMethods(matchLoc))
		{
			Code = 405;
			return (1);
		}
		header["Allow"] = getMethodList();
	        // throw serverConfig::ErrorStatus(405);
		if (checkMaxBody(matchLoc))
		{
			Code = 413;
			return (1);
		}
			// throw serverConfig::ErrorStatus(413);
		if (checkReturn(matchLoc))
		{
			Code = 301;
			return (1);
		}
		if (isCgi(matchLoc) && checkCgi(matchedPath, matchLoc))
		{
			return (1);
		}
			// throw serverConfig::ErrorStatus(301);
	}

}

int	Response::buildBody()
{
	if (request.getBody().length() > server.client_max_body_size)
	{
		Code = 413;
		return (1);
	}
		// throw serverConfig::ErrorStatus(413);
	if (checkLocation())
		return (1);
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
	// strftime(const_cast<char*>(date.c_str()), sizeof(date), "Date: %a, %d %b %Y %H:%M:%S %Z", gmt);
	
	strftime(const_cast<char*>(date.c_str()), sizeof(date), "%a, %d %b %Y %H:%M:%S %Z", gmt);
	header["Date"] = date + "\r\n";
	// response_content.append(date);
	// response_content.append("\r\n");
	
}

// Content-Type: application/json; charset=UTF-8
void	Response::setContentType()
{
	std::string tmp;
	
	tmp.append("Content-Type: ");
	int extnPos = path.rfind(".", std::string::npos);

	if(extnPos != std::string::npos && e_code == 200)
        tmp.append(mimeList.getMimeType(path.substr(extnPos)));
    else
    	tmp.append(mimeList.getMimeType("default"));
    tmp.append("\r\n");
	header["Content-Type"] = tmp;
}

// Content-Length: 155
void	Response::setContentLength()
{
	std::stringstream ss;
	std::string	body_len;

	ss << body.length();
	ss >> body_len;

	header["Content-Length"] = body_len + "\r\n";
	// response_content.append("Content-Length: ");
	// response_content.append(body_len);
	// response_content.append("\r\n");
}

// Server: BestServ (Unix) (Red-Hat/Linux)
void	Response::setServer()
{
	header["Server"] = "nginx 1.0.15\r\n";
	// response_content.append("Server: nginx 1.0.15\r\n");
}

// Connection: close / keep-alive
void	Response::setConnection()
{
    if(request.parse_request[Connection] == "keep-alive")
		header["Connection"] = "keep-alive\r\n";
        // response_content.append("Connection: keep-alive\r\n");
	else
		header["Connection"] = "colse\r\n";
		// response_content.append("Connection: colse\r\n");
}

std::string	Response::getMethodList(Location &matchLoc)
{
	std::vector<int>methods = matchLoc.getAllowedMethods();
	std::string tmp;
	int	i = 0;

	while (i < methods.size())
	{
		if (i > 0)
			tmp.append(", ");
		if (methods.find(methods.begin(), methods.end(), i))
		{
			switch(i)
			{
				case 0:
					tmp.append("GET");
				case 1:
					tmp.append("POST");
				case 2:
				{
					tmp.append("DELETE");
					break;
				}
			}
		}
		i++;
	}
	return (tmp);
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