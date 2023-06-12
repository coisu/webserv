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

parse Request
-> set the env with parsed data
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


