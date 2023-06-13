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
        std::string     path;               // path of requested page/data.
        std::string     location;
        std::string     body;
        size_t          body_len;           // content-Length
        bool            auto_index;
        int             e_code;

    public:
        Mime            mimeList;
        Method          request;            // Method
        ServerConfig    server;             // port & Host
        std::string     response_content;


        Response();
        Response(Request &request);
        ~Response();

        void    buildResponse();
        void    buildErrorResponse();



};

#endif