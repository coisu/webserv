# ifndef MIME_HPP
#define MIME_HPP

#include "http_tcpServer.hpp"

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