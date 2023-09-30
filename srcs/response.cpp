
#include "Response.hpp"

Response::Response()
{
	_target_path = "";
	_location = "";
	_body = "";
	_body_len = 0;
	_auto_index = false;
	_status = -1;
	_response_content = "";
	initialMapStatusCode();
	initialMapHeaders();
}

Response::Response(Request &request, const Server& server ) : _request(request), _server(server)
{
	_target_path = _request.getURL;
	_location = "";
	_body = "";
	_body_len = 0;
	_auto_index = false;
	_status = _request.getCode;
	_response_content = "";
	initStatusCode();
	initHeaders();
}

Response::~Response() {}

bool Response::checkECode()
{
	if (_request.getStatus())
	{
		_status = _request.getStatus();
		return (1);
	}
	return (0);
}
=
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
	this->_headers["Content-Language"] = "";
	this->_headers["Content-Length"] = "";
	this->_headers["Content-Type"] = "";
}

void		Response::initStatusCode()
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
	this->_errorPages[403] = "Forbidden";
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

void Response::processResponse()
{
	std::string	currentMethod(_request.getMethodStr());
	std::string	ext(getExt(_request.getURL()));

	if (_request.getMethodEnum() == DELETE && _status == 404)
		_status = -1;

	setRequestVal();
	setContentType(ext); 
	checkSetLocation(_request.getURL());

	if (_location.getRet().empty())
	{
		if (_status == 0)
		{
			_status = 301;	//MOVED_PERMANENTLY
		}
	}
	else if (_status == -1)
	{
		if (_request.getMethodEnum() == GET || _request.getMethodEnum() == POST)
		{
			if (_request.getMethodEnum() == GET && ext != "php")	//html
			{
				if (_location.getAutoIndex())
				{
					struct stat			fileinfo;
					
					stat(_request.getURL().c_str(), &fileinfo);
					if (S_ISDIR(fileinfo.st_mode))
					{
						_body = writeBodyAutoindex(_request.getURL());		//check url and either make filepath before here.
						_status = 200;
					}
					else if (S_ISREG(fileinfo.st_mode))						//regular file
					{
						_body = fileTextIntoBody(_request.getURL(), mimeList.getMimeType(ext) == "text/html");
						_status = 200;
					}
				}
				else
				{
					_body = writeBodyHtml(_request.getURL(), mimeList.getMimeType(ext) == "text/html");
					_status = 200;
				}
			}
			else
			{
				// _body = writeBodyCgi(_status);
			}
		}
		else if (currentMethod_ == "DELETE")
		{
			_status = execteDelete();
		}
	}
	if (_status >= 400)
	{
		if (checkErrorPage(_status))
			_body = writeBodyHtml(_server.getRoot() + _server.getErrorPage().at(_status), mimeList.getMimeType(ext) == "text/html");
		else
			_body = makeErrorPage(_status);

		_checkCur = "Close";
	}
	// method == post, error page, header create, non-cgi
	if (_request.getMethodEnum() == GET && ext != "php" && _status >= 400)
		_headers += buildHeader(_body.size(), _status);
	else
		_headers += buildHeaderCgi(_body, _status);				// didn't make it yet
}

bool	Response::checkErrorPage(int  status)
{
	// typedef std::vector<std::string>::const_iterator	it_;
	if (_server.getErrorPage().empty())
	{
		return false;
	}
	std::string	status_str = toString(status);
	std::vector<std::string>	vec_(_server.getErrorPageCode());
	std::vector<std::string>::const_iterator itBegin = vec_.begin();
	std::vector<std::string>::const_iterator itEnd = vec_.end();

	for (; itBegin != itEnd; ++itBegin)
	{	
		if (status_str == *(itBegin))
		 return (true);
	}
	return false;
}


std::string		Response::writeBodyHtml(std::string const &filePath, bool isHTML)
{
	std::string		ret;
	std::string		filePath_;
	std::ifstream 	ifs;
	
	filePath_ = "./" + filePath;

	ifs.open(const_cast<char*>(filePath_.c_str()));
	if (ifs.fail())
	{
		return makeErrorPage(404);
	}

	std::string	str;
	while (std::getline(ifs, str))
	{
		if (isHTML) {
			ret += "\r\n";
		}
		else {
			ret += "\n";
		}
		ret += str;
	}

	return (ret);
}

std::string		Response::fileTextIntoBody(const std::string &filepath, bool isHTML)
{
	std::ifstream in(filepath.c_str());
	std::string line;
	std::string ret;

	if (in.is_open())
	{

		while (std::getline(in, line))
		{
			if (isHTML) {
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
	std::string		url
	std::string		filepath;		//check what's in url and set file_path
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

	dir_ptr = opendir(filepath.c_str());
	if (dir_ptr != NULL)
	{
		while ((dir_entry = readdir(dir_ptr))) //get every file and directory info after opendir
		{
			if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
				continue;
			std::string filename = std::string(dir_entry->d_name);
			if (stat((filepath + filename).c_str(), &fileinfo) == 0)
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
	int	idx;
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

	std::map<std::string, std::string> ReqHead = _request.getHead();

	std::map<std::string, std::string>::iterator	itForHeader;
	// std::cout << "=============================" << std::endl;
	for (std::map<std::string, std::string>::iterator it=ReqHead.begin(); it!=ReqHead.end(); ++it)
	{
		itForHeader = _headers.find(it->first);
		if (itForHeader != _headers.end()
			&& itForHeader->first != "Content-Length")
		{
 			_headers[itForHeader->first] = it->second;
			// std::cout << "[ " << itForHeader->first << " ] : " << it->second << std::endl;
		}
	}         
}

void	Response::setContentType(std::string ext)
{
	_header["Content-Type"] = mimeList.getMimeType(ext);
}

bool	Response::checkSetLocation(std::string path)
{
	std::pair<bool, LocationBlock> location_pair;

	location_pair = getMatchLoc(path);
	if (location_pair.first == true)
	{
		setLocation(location_pair.second);
		return (true);
	}
	else
		return (false);
}

std::pair<bool, Location>	Response::getMatchLoc(const std::string& request_path)
{
	Location	ret;
	std::vector<Location>::iterator	curLoc = _server.getLocations.begin();
	std::vector<Location>::iterator	endLoc = _server.getLocations.end();

	for (; curLoc != endLoc; ++curLoc)
	{
		if (curLoc->getPath().length() <= request_path.length()) 
		{
			if (request_path.compare(0, curLoc->getPath().length(), current_location->getPath()) == 0)
				return (std::make_pair(true, *curLoc));
		}
	}
	return (std::make_pair(false, ret));
}

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
	std::string		filePath_("./" + _request.getURL());		//check URL contents

	if (remove(const_cast<char*>(filePath_.c_str())) == -1)
	{
		status = 204;
	}
	return (status);
}

std::string		Response::buildHeader(int bodySize, int status)
{
	std::string	header;

	// body size -> Content-Length
	// append "Content-Lengh" to _headers
	setContentLengh(bodySize);
	// make start line
	header += makeStartLine(status);
	// append headers value!
	header += appendMapHeaders(1, status);
	// time
	header += makeTimeLine(1);

	return (header);
}

void	Response::setContentLengh(int bodySize)
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

// option1 : HTML, 0:CGI
std::string		Response::appendMapHeaders(int option, int statusCode)	
{
	std::string	Header;

	for (std::map<std::string, std::string>::iterator it=_headers.begin(); it!=_headers.end(); it++)
	{
		if ( !(it->second.empty()) )
		{
			if ((request_.getMethod() == "DELETE" && it->first == "Content-Type") 
			|| (it->first == "Content-Type" && option == 0)
			|| (it->first == "Transfer-Encoding"))
			{
				continue ;
			} 
			Header += it->first;
			Header += ": ";
			if (it->first == "Content-Type" && statusCode >= 400)
			{
				Header += "text/html";
			}
			else
			{
				Header += it->second;
			}
			Header += "\r\n";
		}
	}
	return Header;
}

// Date: Thu, 18 Aug 2022 11:02:41 GMT
// option1 : HTML, 0:CGI
std::string		Response::makeTimeLine(int option) 
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
	if (option == 1 || request_.getMethod() == "DELETE")
	{
		timeLine += "\r\n";
	}
	return (timeLine);
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