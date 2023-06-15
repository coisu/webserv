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

#include "Method.hpp"
#include "serverConfig.hpp"
#include "Mime.hpp"
#include "statusCode.hpp"

class Response
{
    private:
        std::string     path;               // path of requested page/data.
        std::string     location;
        std::string     body;
        size_t          body_len;           // content-Length
        bool            auto_index;
        int             Code;

    public:
        Code            statusCode;
        Mime            mimeList;
        Request         request;
        serverConfig    *server;             // port & Host
        std::string     response_content;


        Response();
        Response(Request &request);
        Response(const Response &copy);
        Response &operator=(const Response &copy);
        ~Response();

        void    buildResponse();
        void    buildErrorResponse();

        void	Response::setHeader();
        void    Response::setStatusLine();
        void	Response::setDate();
        void	Response::setContentType();
        void	Response::setContentLength();
        void	Response::setServer();
        void	Response::setConnection();

        void	Response::setBody();


};

#endif