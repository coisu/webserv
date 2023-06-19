# ifndef SERVERCONFIG_HPP
#include SERVERCONFIG_HPP

enum Method
{
    GET,
    POST,
    DELETE
};

class Location
{
    private:
        std::string         path;
        std::string         root;
        std::string         index;
        std::string         retrn;
        std::vector<int>    allow_method;
        std::vector<std::string> cgi_path;
        std::vector<std::string> cgi_ext;
    public:
        std::vector<int>   getAllowMethod ();
};

class serverConfig
{
    private:
        unsigned int                port;
        in_addr_t                   host; // inet_addr
        std::string                 server_name;
        bool                        auto_index;
        std::map<int, std::string>  error_msg;
        std::vector<Location>       location;
        int                         listen_fd;
        
        
};

#endif