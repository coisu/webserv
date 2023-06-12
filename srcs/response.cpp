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
-> read request fd one by one,(if it has problem remove it from the set and close socket)
-> read sucessfully, update last message time.
-> assign server to the client
-> start to build response
	-> pipe and fork, excute CGI
	-> Method GET-read, POST-write, DELETE-delete
	-> build body, html index
	-> append status line amd header contents 

-> check is the host name, port and servername
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

class configServer
{
	private:
		uint16_t						port;
		in_addr_t						host;
		std::string						server_name;
		std::string						root;
		unsigned long					client_max_body_size;
		std::string						index;
		bool							autoindex;
		std::map<int, std::string>		error_pages;
		std::vector<Location> 			locations;
        struct sockaddr_in 				server_addr;
        int     						listen_fd;
};

class Request
{
	private:
        std::string                         path;
        std::string                         query;
        std::map<std::string, std::string>  req_headers;
        Method          	                method;
        size_t                              max_body_size;
        size_t                              body;
        int                               	e_code;
        size_t                              chunk_length;
        std::string                         server_name;
        std::string                         body_str;

};

enum Method
{
    GET,
    POST,
    DELETE
};

class Response
{
	private:
        configServer    server;
        std::string     target_file;
        std::vector<unsigned long> _body;
        size_t          body_length;
        std::string     response_body;
        std::string     location;
        int           	e_code;
        char            *res;
		int				cgi;
		int				cgi_fd[2];
		size_t			cgi_response_length;
        bool            auto_index;

	public:
		std::string     res_content;
        Request     	request;
};

int    Response::buildResponse()
{
    if (request.getBody().length() > server.getClientMaxBodySize())
    {
        e_code = 413;
        return (1);
    }
    if ( checkAllowedMethod() || checkLocation()) 
        return (1);
	if (location.path == cgi-bin)
	{
		check extension (.py || .sh)
		check...
		init env,
		if pipe and fork suceed
		excute CGI..
		execve(CGI->_argv[0], CGI->_argv, CGI->env);
	}
    if (cgi || _auto_index)
        return (0);
    
    if (request.getMethod() == GET)
    {
        std::ifstream file(target_file.c_str());

        if (file.fail())
        {
            _code = 404;
            return (1);
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        response_body = ss.str();
    }
     else if (request.getMethod() == POST)
    {
        if (fileExists(target_file))
        {
            e_code = 204;
            return (0);
        }
        std::ofstream file(target_file.c_str(), std::ios::binary);
        if (file.fail())
        {
            e_code = 404;
            return (1);
        }

        if (request.getMultiformFlag())
        {
            std::string body = request.getBody();
            body = removeBoundary(body, request.getBoundary());
            file.write(body.c_str(), body.length());
        }
        else
        {
            file.write(request.getBody().c_str(), request.getBody().length());
        }
    }
    else if (request.getMethod() == DELETE)
    {
        if (!fileExists(_target_file))
        {
            e_code = 404;
            return (1);
        }
        if (remove( _target_file.c_str() ) != 0 )
        {
            e_code = 500;
            return (1);
        }
    }
    e_code = 200;
    return (0);
}
