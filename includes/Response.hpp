# ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <vector>
#include <string>
#include <map>
#include <set>
#include <sys/stat.h>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <unistd.h>

class Response
{
    private:
        std::string     location;
        std::string     body;
        size_t          body_len;
        bool            auto_index;
        ServerConfig    server;
        int             e_code;


    public:
        Request request;
        std::string     response_content;

        Response();
        Response(Request&);
        ~Response();

        void    buildResponse();
        void    buildErrorResponse();


};

#endif