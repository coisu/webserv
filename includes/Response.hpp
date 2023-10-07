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
#include <dirent.h>
#include <fcntl.h>
#include "Request.hpp"
#include "Mime.hpp"
#include "Server.hpp"
#include "utils.h"
#include "CGI.hpp"

extern   Mime   _mimeList;

class Response
{
    private:
        std::string     _target_path;
        Location        _location;
        std::string     _body;
        std::string     _buffer;
        std::string     _headerStr;
        size_t          _body_len;
        bool            _auto_index;
        int             _status;
        bool            _req_status;
        std::string     _connect;
        std::map<std::string, std::string>     _headers;
        std::map<int, std::string>             _errorPages;
        // Request         _request;
        // Server          _server;

    public:
        // Mime           _mimeList;
        const   Request&        _request;
        Server&         _server;
        t_method	_currentMethod;

        // Response();
        Response( const Request &request, Server& server );
        // Response( int status, const Server& server );
        ~Response();


    void            setServer(const Server &server);
    void            setRequest(const Request &request);
    void	        setStatus(int err);


    /* Init in Constructor */
    void		    initHeaders(void);
    void		    initStatusCode(void);

    /* Process */
    std::string     processResponse(void);
    std::pair<bool, Location>	getMatchLoc(const std::string& request_path);


    void            setLocation(Location Loc);
    void	        setRequestVal(void);

    bool	        checkSetLocation(std::string path);
    std::string     getExt(std::string const &filename) const;
    int	            execteDelete(void);

    /* Body Writer */
    std::string		writeBodyAutoindex(const std::string &str);
    std::string		fileTextIntoBody(bool isHTML);
    std::string		writeBodyHtml(std::string filePath, bool isHTML);
    std::string		makeErrorPage(int	status);
    std::string	    getFileDateTime(time_t sec);

    /* Header Writer */
    std::string		buildHeader(int bodySize, int status);
    std::string		buildHeaderCgi(std::string &body, int status);

    std::string		makeStartLine(int status);
    std::string		makeTimeLine(bool isCGI) ;
    void	        setContentType(std::string ext);
    void            setLocationHeader(void);
    void	        setContentLength(int bodySize);
    std::string		appendMapHeaders(bool isCGI, int statusCode)	;






    /* Utils */
    std::string	toString(const int& i) const;
    void	clear();




};

#endif