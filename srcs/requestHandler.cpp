#include "Response.hpp"

// in server handling, assign response = start request function (return response) inside of this function calling GEThandler etc.
// each hangler returns response class
// construct response class with request as arg > response.requestHandler(serverConfig),
// response.buildResponse()

Response Response::requestHandler(serverConfig &data)
{
    server = date;
    Location *loc = data.location;

    std::ifstream f((server.root + request.parse_request[Path] + "/" + loc->index).c_str());
    request->isDir = (f.good() && !f.rdbuf()->in_avail()) ? true : false;
    f.close();

    std::vector<int>methods = data.location.getAllowedMethods();
    if (request.getMethod() == GET && !methods.find(method.begin(), method.end(), GET)
        || request.getMethod() == POST && !methods.find(method.begin(), method.end(), POST)
        || request.getMethod() == DELETE && !methods.find(method.begin(), method.end(), DELETE))
        code = 405; // throw serverConfig::ErrorStatus(405);
    else if (request.parse_request[path].begin() != '/')
        code = 403; // throw serverConfig::ErrorStatus(403);

    
    
    if (request.getMethod() == GET)
        return handleGET();
    if (request.getMethod() == POST)
        return handlePOST();
    if (request.getMethod() == DELETE)
        return handleDELETE();
}

static bool isDirectory(const std::string &name)
{
    return (*name.rbegin() == '/') ? true : false;
}