#include "Response.hpp"

// construct response class with request as arg > response.requestHandler(serverConfig),
// response.buildResponse()

Response::requestHandler(serverConfig &data)
{
    server = date;
    std::vector<int>methods = data.location.getAllowedMethods();
    if (request.getMethod() == GET && methods.find(method.begin(), method.end(), GET)
        || request.getMethod() == POST && methods.find(method.begin(), method.end(), POST)
        || request.getMethod() == DELETE && methods.find(method.begin(), method.end(), DELETE))
        code = 405; // throw serverConfig::ErrorStatus(405);
    if (request.parse_request[path].begin() != '/')
        code = 403; // throw serverConfig::ErrorStatus(403);
    if (request.getMethod() == GET)
        handleGET();
}