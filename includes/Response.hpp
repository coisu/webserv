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
        std::string     location;
        std::string     body;
        size_t          body_len;
        bool            auto_index;
        int             e_code;

    public:
        Request         request;
        ServerConfig    server;
        std::string     response_content;

        Response();
        Response(Request&);
        ~Response();

        void    buildResponse();
        void    buildErrorResponse();



};

#endif