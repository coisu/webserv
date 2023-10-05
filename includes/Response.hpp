# ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <vector>
#include <string>
#include <map>
#include <set>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "Request.hpp"
#include "Mime.hpp"
#include "Server.hpp"

class Request;
class Mime;
class Server;


class Response
{
    private:
        std::string     _target_path;
        Location        _location;
        std::string     _body;
        std::string     _buffer;
        std::string     _headerStr;
        size_t          _body_len;
        bool            _auto_index;
        int             _status;
        bool            _req_status;
        std::string     _connect;
        std::map<std::string, std::string>     _headers;
        Request         _request;
        Server          _server;

    public:
        Mime            _mimeList;
        // Request         _request;
        // Server          _server;

        Response();
        // Response( Request &request, const Server& server );
        // Response( int status, const Server& server );
        ~Response();


    void    setServer(const Server &server);
    void    setRequest(const Request &request);


    /* Constructor */
    void		initHeaders(void);
    void		initStatusCode(void);
    /* Process */
    std::string    processResponse();

    /* Body Writer */

    /* Header Write */

    void	clear();




};

#endif