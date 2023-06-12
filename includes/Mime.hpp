# ifndef MIME_HPP
#define MIME_HPP

#include "http_tcpServer.h"

class Mime
{
    private:
        std::map<std::string, std::string> mimeType;
    public:
        Mime();
        ~Mime();
        std::string getType(std::string extn);
};

#endif