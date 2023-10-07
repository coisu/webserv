
#include "Response.hpp"


Mime   _mimeList;
// Response::Response()
// {
// 	_target_path = "";
// 	_body = "";
// 	_buffer = "";
// 	_headerStr = "";
// 	_body_len = 0;
// 	_auto_index = false;
// 	_status = 0;
// 	_req_status = false;
// 	_connect = "";
// 	// _response_content = "";
// 	initStatusCode();
// 	initHeaders();
// }


// void    Response::setServer(const Server &server)
// {
// 	this->_server = server;
// }

// void    Response::setRequest(const Request &request)
// {
// 	this->_request = request;
// 	this->_target_path = _request->getLocPath();
// }


Response::Response(const Request &request, Server &server ) : _request(request), _server(server)
{
	_target_path = _request.getLocPath();
	_body = "";
	_buffer = "";
	_headerStr = "";
	_body_len = 0;
	_auto_index = false;
	_status = 0;
	_req_status = false;
	_connect = "Keep-Alive";
	// _response_content = "";
	initStatusCode();
	initHeaders();
}

Response::~Response() {}

// Response::Response(int status, const Server& server) :  _server(server)
// {
// 	_target_path = setTargetPath();
// 	_body = "";
// 	_buffer = ""
// 	_headerStr = ""
// 	_body_len = 0;
// 	_req_status = true;
// 	_status = status;
// 	_connect = "";
// 	initStatusCode();
// 	initHeaders();
// }

void		Response::initHeaders(void)
{
	this->_headers["Allow"] = "";
	this->_headers["Content-Location"] = "";
	this->_headers["Last-Modified"] = "";
	this->_headers["Location"] = "";
	this->_headers["Retry-After"] = "";
	this->_headers["Server"] = "";
	this->_headers["Transfer-Encoding"] = "";
	this->_headers["WWW-Authenticate"] = "";
	this->_headers["Content-Encoding"] = "";
	this->_headers["Content-Language"] = "";
	this->_headers["Content-Length"] = "";
	this->_headers["Content-Type"] = "";
}

void		Response::initStatusCode(void)
{
	this->_errorPages[100] = "Continue";
	this->_errorPages[101] = "Switching Protocols";
	this->_errorPages[103] = "Early Hints";
	this->_errorPages[200] = "OK";
	this->_errorPages[201] = "Created";
	this->_errorPages[202] = "Accepted";
	this->_errorPages[203] = "Non-Authoritative Information";
	this->_errorPages[204] = "No Content";
	this->_errorPages[205] = "Reset Content";
	this->_errorPages[206] = "Partial Content";
	// redirection request
	this->_errorPages[300] = "Multiple Choices";
	this->_errorPages[301] = "Moved Permanently";
	this->_errorPages[302] = "Found";
	this->_errorPages[303] = "See Other";
	this->_errorPages[304] = "Not Modified";
	this->_errorPages[305] = "Use Proxy";
	this->_errorPages[306] = "unused";
	this->_errorPages[307] = "Temporary Redirect";
	this->_errorPages[308] = "Permanent Redirect";
	// Client's Request error
	this->_errorPages[400] = "Bad Request";
	this->_errorPages[401] = "Unauthorized";
	this->_errorPages[402] = "Payment Required";
	this->_errorPages[403] = "Forbidden"; 	//POST to exit file but without prtmit
	this->_errorPages[404] = "Not Found";
	this->_errorPages[405] = "Method Not Allowed";	
	this->_errorPages[408] = "Request Timeout";	
	this->_errorPages[411] = "Length Required";	
	this->_errorPages[413] = "Payload Too Large";	
	this->_errorPages[414] = "URI Too Long";
	// Server error
	this->_errorPages[500] = "Internal Server Error";	
	this->_errorPages[502] = "Bad Gateway";	
	this->_errorPages[505] = "HTTP Version Not Supported";	
}


// ERROR MANAGING: 405, 413


// static bool isAllowedMethod(HttpMethod &method, Location &location, short &code)
// {
//     std::vector<short> methods = location.getMethods();
//     if ((method == GET && !methods[0]) || (method == POST && !methods[1]) ||
//        (method == DELETE && !methods[2])|| (method == PUT && !methods[3])||
//         (method == HEAD && !methods[4]))
//     {
//         code = 405;
//         return (1);
//     }
//     return (0);
// }

// static std::string combinePaths(std::string p1, std::string p2, std::string p3)
// {
//     std::string res;
//     int         len1;
//     int         len2;

//     len1 = p1.length();
//     len2 = p2.length();
//     if (p1[len1 - 1] == '/' && (!p2.empty() && p2[0] == '/') )
//         p2.erase(0, 1);
//     if (p1[len1 - 1] != '/' && (!p2.empty() && p2[0] != '/'))
//         p1.insert(p1.end(), '/');
//     if (p2[len2 - 1] == '/' && (!p3.empty() && p3[0] == '/') )
//         p3.erase(0, 1);
//     if (p2[len2 - 1] != '/' && (!p3.empty() && p3[0] != '/'))
//         p2.insert(p1.end(), '/');
//     res = p1 + p2 + p3;
//     return (res);
// }


std::string Response::processResponse()
{
	// std::string	currentMethod(_request.getMethodEnum());
	_currentMethod = _request.getMethodEnum();
	std::string	ext(getExt(_request.getLocPath()));
	// std::string _headerStr = "";
	// std::string buffer = "";
	bool isRedirect = false;

	if (_currentMethod == DELETE && _status == 404)
		_status = -1;

	// setRequestVal();
	setContentType(ext); 
	checkSetLocation(_target_path);

	// if (_location.getRet().empty())
	// {
	// 		std::cout << "-----?TESTING?" << std::endl;

	// 	if (_status == 0)
	// 	{
	// 		_status = 301;	//MOVED_PERMANENTLY
	// 		isRedirect = true;
	// 	}
	// }
if (_status == -1)
	{
		if (_currentMethod == GET || _currentMethod == POST)
		{
			if (_currentMethod == GET && ext != "php")	//html
			{
				if (_location.getAutoIndex())
				{
					struct stat			fileinfo;

					stat(_target_path.c_str(), &fileinfo);
					if (S_ISDIR(fileinfo.st_mode))
					{
						_body = writeBodyAutoindex(_request.getURL());
						_status = 200;
					}
					else if (S_ISREG(fileinfo.st_mode))						//regular file
					{
						_body = fileTextIntoBody(_mimeList.getMimeType(ext) == "text/html");
						_status = 200;
					}
				}
				else
				{
					_body = writeBodyHtml(_target_path, _mimeList.getMimeType(ext) == "text/html");
					_status = 200;
				}

			}
			else
			{
				if (_currentMethod == POST)
				{
					struct stat			fileinfo;
					int ret;

					ret = stat(_target_path.c_str(), &fileinfo);
					if (ret != 0)
					{
						std::string reqBody = _request.getBody();
						int	fd = open(_target_path.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);
						if (fd > 0 && reqBody.length() && write(fd, reqBody.c_str(), reqBody.length()) > 0)
						{
							std::cout << "Created by POST of " << _target_path <<std::endl;
						}
						close(fd);
						_status = 201;
					}

				}
				if (_location.getIsCGI())
				{
					CGI	cgi(_server, _request.getURL(), _request.getMethodStr(), _location.getCGIConfig());
					_body = cgi.exec_cgi();
				}

			}
		}
		else if (_currentMethod == DELETE)
		{
			_status = execteDelete();
		}
	}
		std::cout << "?TESTING?" << std::endl;

	if (_status >= 400)
	{
		std::map<int, std::string> ep = _server.getErrorPages();
		if (ep.find(_status) != ep.end())
			_body = writeBodyHtml(_server.getRoot() + ep[_status], _mimeList.getMimeType(ext) == "text/html");
		else
			_body = makeErrorPage(_status);
		_connect = "Close";
	}
	
	/* REDIRECTION HEADER */
	if ((!_location.getRet().empty() && isRedirect) || _status == 201)
	{
		setLocationHeader();
	}

	if (!_request.getHead()["Connection"].empty())
	{
		if (_request.getHead()["Connection"] == "close")
		{
			_connect = "Close";
			this->_headers["Connection"] = _connect;
		}
	}
	else if (_status < 400)
	{
		_connect = "Keep-Alive";
		this->_headers["Connection"] = _connect;
	}
	if (_status == 0)
	{
		_status = 200;
	}
	/* MAKE HEADER */
	if ((_currentMethod == GET && ext != "php") || _status >= 400)
		_headerStr += buildHeader(_body.size(), _status);
	else
		_headerStr += buildHeaderCgi(_body, _status);				// didn't make it yet

	_buffer = (_currentMethod = DELETE) ? _headerStr + "\r\n" : _headerStr + _body + "\r\n";
		

	std::cout << "******************************************\n";
	std::cout << _buffer << std::endl;
	std::cout << "******************************************\n";
	return _buffer;
}

std::string		Response::writeBodyHtml(std::string filePath, bool isHTML)
{
	std::string		ret;
	std::ifstream 	ifs;
	
	// if (path[0] != '/')
	// 	filePath = "/" + path;

	ifs.open(const_cast<char*>(filePath.c_str()));
	if (ifs.fail())
	{
		return makeErrorPage(404);
	}

	std::string	str;
	while (std::getline(ifs, str))
	{
		if (isHTML)
		{
			ret += "\r\n";
		}
		else
		{
			ret += "\n";
		}
		ret += str;
	}

	return (ret);
}

std::string		Response::fileTextIntoBody(bool isHTML)
{
	std::ifstream in(_target_path.c_str());
	std::string line;
	std::string ret;

	if (in.is_open())
	{

		while (std::getline(in, line))
		{
			if (isHTML)
			{
				ret += "\n";
				ret += line + "</br>";
			}
			else
			{
				ret += "\n";
				ret += line;
			}
		}
	}
	return (ret);
}

std::string		Response::writeBodyAutoindex(const std::string &str)
{
	std::string 	ret;
	std::string		url;
	DIR				*dir_ptr;
	struct dirent  	*dir_entry;
	struct stat		fileinfo;
	std::stringstream	ss;

	url = str;
	if (!(*(url.rbegin()) == '/'))
		url.append("/");

	ret += "\r\n";
	ret += "<!DOCTYPE html>\r\n";
	ret += "<html lang=\"en\">\r\n";
	ret += "<head>\r\n";
	ret += "<meta charset=\"UTF-8\">\r\n";
	ret += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
	ret += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
	ret += "<title>\r\n";
	ret += "Index of " + url + "\r\n";
	ret += "</title>\r\n";
	ret += "</head>\r\n";
	ret += "<body>\r\n";
  	ret += "<h1>Index of " + url + "</h1><hr><pre>\r\n";
  	ret += "<a href=\"../\">../</a>\r\n";

	dir_ptr = opendir(_target_path.c_str());
	if (dir_ptr != NULL)
	{
		while ((dir_entry = readdir(dir_ptr))) //get every file and directory info after opendir
		{
			if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
				continue;
			std::string filename = std::string(dir_entry->d_name);
			if (stat((_target_path + filename).c_str(), &fileinfo) == 0)
			{
				if (S_ISDIR(fileinfo.st_mode))
				{
					filename += "/";
					ret += "<a href=\"" + url + filename + "\">" + filename + "</a>";
				}
				else
					ret += "<a href=\"" + url + filename + "\">" + filename + "</a>";
			}
			ret += std::string(30 - filename.size(), ' ');
			ret += getFileDateTime(fileinfo.st_mtime);
			std::string filesize;
			if (S_ISDIR(fileinfo.st_mode))
				filesize = "-";
			else
			{
				ss << fileinfo.st_size;
				filesize = ss.str();
				ss.str("");
			}
			ret += std::string(10, ' ');
			ret += filesize;
			ret += "\r\n";
		}
	}
	ret += "<hr></pre>\r\n";
	ret += "</body>\r\n";
	ret += "</html>\r\n";
	closedir(dir_ptr);
	return (ret);
}

std::string	Response::getExt(std::string const &filename) const
{
	std::string	ext;
	std::string::size_type	idx;
	idx = filename.rfind(".");
	if (idx != std::string::npos) {
		ext = filename.substr(idx + 1);
	}
	else
		ext = "default";
	//std::cout << ext << " is ext in getExit" << std::endl;
	//std::cout << filename << " is filename in getExt"  << std::endl;
    return ext;
}

void	Response::setRequestVal(void)
{
	std::map<std::string, std::string> reqHead = _request.getHead();
	std::map<std::string, std::string>::iterator it=reqHead.begin();

	std::cout << "===========HERE==================" << std::endl;
		int i = 0;

	for (; it != reqHead.end(); ++it )
	{

		++i;
		std::cout << "EXIT?? - " << toString(i) << std::endl;

		std::cout << "now iterator is" << "[ " << it->first << " ] : " << it->second << std::endl;

		std::map<std::string, std::string>::iterator new_it = _headers.find(it->first);
		if (new_it != _headers.end())
			std::cout << "return it Find" << "[ " << new_it->first << " ] : " << new_it->second << std::endl;

		// if (it->first != "Content-Length" && _headers.find(it->first) != _headers.end())
		// {
		// 	std::cout <<"IN?????" <<std::endl;
		// 	_headers[it->first] = it->second;
		// 	std::cout << "[ " << it->first << " ] : " << it->second << std::endl;
		// }
	}

	// std::map<std::string, std::string>::iterator	itForHeader;
	// // std::map<std::string, std::string>::iterator	it = ReqHead.begin();

	// std::cout << "===========HERE==================" << std::endl;
	// for (std::map<std::string, std::string>::iterator it = ReqHead.begin(); it != ReqHead.end(); ++it)
	// {
	// 	// itForHeader = _headers.find(it->first);
	// 	// if (itForHeader != _headers.end() && itForHeader->first != "Content-Length")
	// 	// {
 	// 		_headers[itForHeader->first] = it->second;
			// std::cout << "[ " << itForHeader->first << " ] : " << it->second << std::endl;
	// 	}
	// }         
}

void	Response::setContentType(std::string ext)
{
	_headers["Content-Type"] = _mimeList.getMimeType(ext);
}

bool	Response::checkSetLocation(std::string path)
{
	std::pair<bool, Location> location_pair;


	// location_pair = getMatchLoc(path);
	location_pair = _server.srchLocation(path);
	if (location_pair.first == true)
	{
		setLocation(location_pair.second);
		return (true);
	}
	else
		return (false);
}

void	Response::setStatus(int err)
{
	this->_status = err;
}

// std::pair<bool, Location>	Response::getMatchLoc(const std::string& request_path)
// {
// 	size_t match = 0;
// 	Location	ret;

// 	std::vector<Location>::iterator cur = _server.getLocations().begin();
// 	std::vector<Location>::iterator end = _server.getLocations().end();

// std::cout << "\n\n\n";

// std::cout << "REQUEST PATH : " << request_path << std::endl;
// 	for (; cur != end; ++cur)
// 	{
// 		std::cout << "CHECKING.... " << cur->getPath() << "??" << std::endl;

// 	}


	// std::cout << "CHECKING... " << _server.getLocations().begin()->getPath() << std::endl;


	// for (std::vector<Location>::iterator it = _server.getLocations().begin(); it != _server.getLocations().end(); ++it) 
	// {
	// 	std::cout << "it Path : " << it->getPath() << std::endl;
	// 	if (request_path.find(it->getPath()) == 0)
	// 	{
	// 		// if( it->getPath() == "/" || request_path.length() == it->getPath().length() || request_path[it->getPath().length()] == '/')
	// 		// {
	// 			if(it->getPath().length() > match)
	// 			{
	// 				match = it->getPath().length();
	// 				ret = *it;
	// 			}
	// 		// }
	// 	}
	// }
// 	if (match > 0)
// 		return (std::make_pair(true, ret));
// 	return (std::make_pair(false, ret)); 
// }

void Response::setLocation(Location Loc)
{
	_location = Loc;
}

std::string		Response::makeErrorPage(int	status)
{
	std::string	errorMessage(_errorPages[status]);
	std::string	errorBody;

	errorBody += "\r\n";
	errorBody += "<!DOCTYPE html>\r\n";
	errorBody += "<html lang=\"en\">\r\n";
	errorBody += "<head>\r\n";
	errorBody += "	<meta charset=\"UTF-8\">\r\n";
	errorBody += "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
	errorBody += "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
	errorBody += "	<title>";
	errorBody += toString(status);
	errorBody += "</title>\r\n";
	errorBody += "</head>\r\n";
	errorBody += "<body>\r\n";
	errorBody += "	<h1>";
	errorBody += toString(status);
	errorBody += "</h1>\r\n";
	errorBody += "	<h1>";
	errorBody += errorMessage;
	errorBody += "</h1>\r\n";
	errorBody += "</body>\r\n";
	errorBody += "</html>";
	
	return errorBody;
}

int	Response::execteDelete(void)
{
	int	status(200);

	if (remove(const_cast<char*>(_target_path.c_str())) == -1)
	{
		status = 204;
	}
	return (status);
}

std::string		Response::buildHeader(int bodySize, int status)
{
	std::string	header;

	setContentLength(bodySize);
	header += makeStartLine(status);
	header += makeTimeLine(false);
	header += appendMapHeaders(false, status);
	

	return (header);
}

std::string		Response::buildHeaderCgi(std::string &body, int status)
{
	std::string	header;
	std::string tmp(body);
	std::string::size_type n;

	header += makeStartLine(status);

	n = tmp.find("\r\n\r\n");
	if (n != std::string::npos)
	{
		header += tmp.substr(0, n) + "\r\n";
		body.clear();
		body = tmp.substr(n, tmp.size());
	}

	if (_request.getMethodEnum() == DELETE)
	{
		setContentLength(0);
	}
	else
	{
		setContentLength(body.size() - 2);
	}
	header += appendMapHeaders(true, status);
	header += makeTimeLine(true);

	return (header);
}

void	Response::setContentLength(int bodySize)
{
	this->_headers["Content-Length"] = toString(bodySize);
}

// HTTP/1.0 400 Bad Request
std::string		Response::makeStartLine(int status)
{
	std::string	startLine;
	std::string	statusMessage(_errorPages[status]);

	startLine += "HTTP/1.1";
	startLine += " ";
	startLine += toString(status);
	startLine += " ";
	startLine += statusMessage;
	startLine += "\r\n";	
	return startLine;
}


std::string		Response::appendMapHeaders(bool isCGI, int statusCode)	
{
	std::string	_headerStr;

	for (std::map<std::string, std::string>::iterator it=_headers.begin(); it!=_headers.end(); it++)
	{
		if ( !(it->second.empty()) )
		{
			if ((_request.getMethodEnum() == DELETE && it->first == "Content-Type") 
			|| (it->first == "Content-Type" && isCGI == true)
			|| (it->first == "Transfer-Encoding"))
			{
				continue ;
			} 
			_headerStr += it->first;
			_headerStr += ": ";
			if (it->first == "Content-Type" && statusCode >= 400)
			{
				_headerStr += "text/html";
			}
			else
			{
				_headerStr += it->second;
			}
			_headerStr += "\r\n";
		}
	}
	return _headerStr;
}

// Date: Thu, 18 Aug 2022 11:02:41 GMT
std::string		Response::makeTimeLine(bool isCGI) 
{
	std::string	timeLine;
	timeLine += "Date: ";
	
  	time_t rawtime;
  	struct tm* timeinfo;
  	char buffer[80];

  	time(&rawtime);
  	timeinfo = localtime(&rawtime);

  	strftime(buffer, 80, "%a, %d %b %Y %T GMT", timeinfo);

	timeLine += buffer;
	if (isCGI == false || _request.getMethodEnum() == DELETE)
	{
		timeLine += "\r\n";
	}
	return (timeLine);
}

std::string	Response::getFileDateTime(time_t sec)
{
	std::string	ret;
	char		buf[18];

	strftime(buf, sizeof(buf), "%d-%b-%Y %H:%M", localtime(&sec));
	ret += buf;

	return (ret);
}

void Response::setLocationHeader(void)
{
	std::string url = _request.getURL();

	this->_headers["Location"] = url;
}

void	Response::clear()
{
	this->_buffer.clear();
	this->_headerStr.clear();
	this->_body.clear();
}

std::string	Response::toString(const int& i) const
{
	std::ostringstream ss;
	ss << i;
	return (ss.str());
}

// bool Response::buildBody()
// {
// 	if (_request.getBody().length() > _server.getClientMaxBodySize())
// 	{
// 		_status = 413;
// 		return (1);
// 	}
// 	if (set)
// }

// int	Response::buildResponse() 
// {
// 	if (checkECode() || buildBody())
// 		buildErrorBody();
// }