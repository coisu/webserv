# ifndef STATUSCODE_HPP
#define STATUSCODE_HPP

#include "http_tcpServer.h"

class Code
{
    private:
        std::map<int, std::string> code;
    public:
        Code();
        ~Code();
        std::string getCodeMessage(int code);
};

#endif