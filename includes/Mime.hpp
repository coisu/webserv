# ifndef MIME_HPP
#define MIME_HPP

// #include "http_tcpServer.hpp"
// #include "Response.hpp"

class Mime
{
    private:
        std::map<std::string, std::string> mimeType;

    public:
        Mime();
        ~Mime();
        Mime &operator=(Mime const &copy);
        std::string getMimeType(std::string extn);
};

#endif