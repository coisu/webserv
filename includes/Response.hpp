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

class Response
{
    private:
        std::string     path;               // path of requested page/data.
        std::string     location;
        std::string     body;
        size_t          body_len;           // content-Length
        bool            auto_index;
        int             e_code;

    public:
        Mime            mimeList;
        Method          request;            // Method
        serverConfig    server;             // port & Host
        std::string     response_content;


        Response();
        Response(Method &request);
        ~Response();

        void    buildResponse();
        void    buildErrorResponse();

        void	Response::writeDate();
        void	Response::writeContentType();
        void	Response::writeContentLength();






};

#endif