# ifndef MIME_HPP
#define MIME_HPP

// #include "http_tcpServer.hpp"
// #include "Response.hpp"
#include <map>
#include <string>

class Mime
{
    private:
        std::map<std::string, std::string> mimeType;

    public:
        Mime();
        ~Mime();
        std::string getMimeType(std::string extn);
};

#endif