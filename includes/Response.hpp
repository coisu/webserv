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

class Response
{
    private:
        std::string     _target_path;
        Location        _location;
        std::string     _body;
        size_t          _body_len;
        bool            _auto_index;
        int             _status;
        bool            _req_status;
        std::string     _checkCur;
        std::map<std::string, std::string>     _headers;

    public:
        Mime            _mimeList;
        Request         _request;
        ServerConfig    _server;
        


        Response();
        Response( Request &request, const Server& server );
        Response( int status, const Server& server );
        ~Response();

    /* Constructor */
    void		Response::initHeaders(void);
    void		Response::initStatusCode(void);
    /* Process */
    void    Response::processResponse();

    /* Body Writer */

    /* Header Write */



};

#endif