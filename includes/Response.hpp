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
        std::string     content;
        std::string     body;
        std::string     location;
        size_t          body_len;
        Server          server;
        bool            auto_index;

    public:
        Request request;


}