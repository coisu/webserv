
#include "Response.hpp"

Mime   _mimeList;

Response::Response(int status, Request &r, Server &s) : _request(r), _server(s)
{
	_target_path = "";
	_body = "";
	_buffer = "";
	_headerStr = "";
	_body_len = 0;
	_auto_index = false;
	_status = -1;
	_req_status = false;
	_connect = "Keep-Alive";
	initStatusCode();
	initHeaders();
	std::string responseBuffer = jumpToErrorPage(status);
	std::stringstream ss;
	ss << "==============EMPTY CONFIG RESPONSE==============\n";
	ss << responseBuffer << std::endl;
	ss << "=================================================\n";
	ft_logger(ss.str(), DEBUG, __FILE__, __LINE__);
}

Response::Response(const Request &request, Server &server ) : _request(request), _server(server)
{
	// _target_path = _server.getRoot() + "/";
	_target_path = _request.getLocPath();
	ft_logger("target path: " + _target_path, DEBUG, __FILE__, __LINE__);
	_body = "";
	_headerStr = "";
	_body_len = 0;
	_auto_index = false;
	_status = -1;
	_return = -1;
	_req_status = false;
	_connect = "Keep-Alive";
	initStatusCode();
	initHeaders();
}

Response::~Response() {}

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

bool Response::isAllowedMethod(int currentMethod)
{
    std::vector<int> methods = _location.getAllowMethods();

	if (methods.size() == 0)
	{
		return (1);
	}
	if (find(methods.begin(), methods.end(), currentMethod) == methods.end())
    {
        _status = 405;
        return (0);
    }
	return (1);
}

std::string Response::jumpToErrorPage(int status)
{
	std::string errorBody;
	std::string errorHeader;

	errorBody = makeErrorPage(status);
	setContentType("default");
	errorHeader = buildHeader(errorBody.size(), status);

	return (errorHeader + errorBody + "\r\n");
}

std::string Response::processResponse(int &cgi_fd, int &cgi_pid)
{
	_currentMethod = _request.getMethodEnum();
	bool isRedirect = false;

	if (_currentMethod == DELETE && _status == 404)
		_status = -1;
	if (_request.getBody().length() > _server.getClientBodySize())
	{
		std::stringstream ss;
		ss << "_request.getBody() : " << _request.getBody() << std::endl;
		ss << "clien max body size : " << _server.getClientBodySize() << std::endl << std::endl;
		ft_logger(ss.str(), DEBUG, __FILE__, __LINE__);
		setStatus(413);
	}
	/* copy if there are same header from request */
	setRequestVal();

	if(!checkSetLocation(_target_path))
	{
		Location L("location/;index:index.html;allow_methods:DELETE,POST,GET;autoindex:off;");
		ft_logger("Location not set, set default location", DEBUG, __FILE__, __LINE__);
		setLocation(L);
	}

	setTargetPath();

	std::string ext = getExt(_target_path);
	if ((_headers["Content-Type"] == "" && pathIsDir(_target_path) != IS_DIR)\
		|| (pathIsDir(_target_path) == IS_DIR && _location.getAutoIndex()))
		setContentType(ext); 

	/* handle return */
	if (_location.getRet() == "")
	{
		if (pathIsDir(_target_path) == IS_DIR && !_location.getAutoIndex())
			setStatus(403);
	}
	else
	{
		int	code = 0;
		std::string str = "";
		if (pathIsDir(_target_path) != IS_REG) // serve return
		{
			if (isNumeric(_location.getRet()))
			{
				std::stringstream ssint;
				ssint << _location.getRet();
				ssint >> code;
			}
			else
			{
				str = _location.getRet();
				// setStatus(302);
				_status = 302;
				isRedirect = true;
			}
			if (code != 0)
			{
				_return = code;
				// setStatus(code);
				if (_location.getIndex() == "" && !_location.getAutoIndex())
					setStatus(code);
			}		
			if (_status == 301 || _status == 302 || _status == 303 || _status == 307 || _status == 308)
			{
				if (!str.empty())
					setLocationHeader(str);
				else
					setLocationHeader(" ");
			}
			if (_status == -1 && _req_status == false && _return == -1 && !isRedirect)
			{
				_status = 301;	//MOVED_PERMANENTLY
				isRedirect = true;
			}
		}
	}
			
	if (isAllowedMethod(_currentMethod) && (_status == -1))
		buildBodywithMethod(ext, cgi_fd, cgi_pid);
	if (_status >= 400)
		buildErrorBody(ext);	

	/* REDIRECTION HEADER */
	if ((!_location.getRet().empty() && isRedirect && _req_status == false) || _status == 201)
	{
		if (_headers["Location"] == "")
			setLocationHeader(_request.getURL());
	}
	/* GENERAL HEADER MAP */
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

	/* MAKE HEADER */
	if ((_currentMethod != POST && !_location.getIsCGI()) || _status >= 400 || (!_location.getIsCGI() && _status == _return))
	{
		_headerStr += buildHeader(_body.size(), _status);
		_buffer = (_body == "") ? _headerStr + "\r\n\r\n" : _headerStr + _body + "\r\n";
	}
	std::stringstream ss;
	ss << "__________________RESPONSE___________________\n" << _buffer << "\n______________________________________________\n";
	ft_logger(ss.str(), DEBUG, __FILE__, __LINE__);
	return _buffer;
}

void Response::setTargetPath()
{
	/* CASE: alias O */
	std::string uri = _request.getURL();
	std::string resource = uri.substr(0, uri.find_first_of('?'));
	if (_location.getAlias().find(resource) != std::string::npos)
		resource = "";
	if (_location.getAlias() != "")		// manage case Alias Exist
	{
		resource.erase(0, _location.getPath().length());
		resource = resource[0] != '/' ? ("/" + resource) : resource;
		int ret = pathIsDir(_server.getRoot() + _location.getAlias() + resource);
		if (_location.getIndex() == "")
			_target_path = _server.getRoot() + _location.getAlias() +resource;
		else							// Alias with index
		{		
			if (ret == IS_DIR)
			{
				if (!_location.getAutoIndex())
					_target_path = _server.getRoot() + _location.getAlias() + resource + _location.getIndex();
				else
					_target_path = _server.getRoot() + _location.getAlias() + resource;
			}
			else
				_target_path =  _server.getRoot() + _location.getAlias() + resource;
		}
	}
	/* alias not exists*/
	else 
	{
		/* CASE: alias X, index O */
		if (_location.getIndex() != "")
		{
			if(pathIsDir(_target_path) == IS_DIR && !_location.getAutoIndex())
				_target_path += _location.getIndex();
		}
	}
	ft_logger("[ Directive Path ] " + _target_path, DEBUG, __FILE__, __LINE__);
}

void Response::buildBodywithMethod(std::string ext, int &cgi_fd, int &cgi_pid)
{

	if (_currentMethod == GET || _currentMethod == POST)
	{
		if (_currentMethod == GET && !_location.getIsCGI())	//html
		{
			if (_location.getAutoIndex())
			{

				int ret = pathIsDir(_target_path);

				if (ret == N_FOUND)
					_status = _return == -1 ? 404 : _return;
				else if (ret == IS_DIR)
					_body = writeBodyAutoindex(_request.getURL());
				else if (ret == IS_REG)						//regular file
					_body = fileTextIntoBody(_mimeList.getMimeType(ext) == "text/html");
				else
					_status = _return == -1 ? 403 : _return;
			}
			else
			{
				std::pair<bool, std::string> body_pair;
				body_pair = writeBodyHtmlPair(_target_path, _mimeList.getMimeType(ext) == "text/html");
				if (body_pair.first == true)
				{
					if (_status == -1)
						_status = 200;
					_body = body_pair.second;
				}
			}
		}
		else
		{
			if (_location.getIsCGI())
			{
				CGI	cgi(_server, _location, _request);
				cgi.exec_cgi(cgi_fd, cgi_pid);
			}
			else
			{
				_return == -1 ? (_status = 405) : (throw _return);
				_body = "CGI is not set";
			}
		}
	}
	else if (_currentMethod == DELETE)
	{
		_status = execteDelete();
	}
}


void Response::buildErrorBody(std::string ext)
{
	if (_status >= 400)
	{
		ft_logger("status -- " + toString(_status), DEBUG, __FILE__, __LINE__);
		std::map<int, std::string> ep = _server.getErrorPages();
		if (ep.find(_status) != ep.end())
		{
			ft_logger("making error page with directive file", DEBUG, __FILE__, __LINE__);
			_body = writeBodyHtml(_server.getRoot() + ep[_status], _mimeList.getMimeType(ext) == "text/html");
		}
		else
			_body = makeErrorPage(_status);

		_connect = "Close";
	}

}

std::string Response::buildErrorBody(int err)
{

	if (_status >= 400)
	{
		std::map<int, std::string> ep = _server.getErrorPages();
		if (ep.find(err) != ep.end())
		{
			ft_logger("making error page with directive file", DEBUG, __FILE__, __LINE__);
			_body = writeBodyHtml(_server.getRoot() + ep[err], true);
		}
		else
			_body = makeErrorPage(err);

		_connect = "Close";
	}
	return (_body);
}

std::pair<bool, std::string>	Response::writeBodyHtmlPair(std::string filePath, bool isHTML)
{
	std::string		ret;
	std::ifstream 	ifs;
	
	// if (path[0] != '/')
	// 	filePath = "/" + path;
	ifs.open(const_cast<char*>(filePath.c_str()));
	
	if (ifs.fail())
	{
		ifs.close();
		if (pathIsDir(filePath) == N_FOUND)
			_status = _return == -1 ? 404 : _return;
		else if (!(isPermit(filePath) & READABLE))
			_status = _return == -1 ? 403 : _return;
		return (std::make_pair(false, ""));
	}
	std::string	str;
	while (std::getline(ifs, str))
	{
		if (isHTML)
			ret += "\r\n";
		else
			ret += "\n";
		ret += str;
	}
	ifs.close();

	return (std::make_pair(true, ret));
}

std::string		Response::writeBodyHtml(std::string filePath, bool isHTML)
{
	std::string		ret;
	std::ifstream 	ifs;
	
	ifs.open(const_cast<char*>(filePath.c_str()));
	if (ifs.fail())
	{
		ifs.close();
		return makeErrorPage(_status);
	}
	std::string	str;
	while (std::getline(ifs, str))
	{
		if (isHTML)
			ret += "\r\n";
		else
			ret += "\n";
		ret += str;
	}
	ifs.close();

	return (ret);
}

std::string		Response::fileTextIntoBody(bool isHTML)
{
	std::ifstream in(_target_path.c_str());
	std::string line;
	std::string ret;

	if (!(isPermit(_target_path) & READABLE) || !(isPermit(_target_path) & EXCUTABLE))
	{
		_status = _return == -1 ? 403 : _return;		
		return "";
	}
	if (in.is_open())
	{
		ft_logger("file opened", DEBUG, __FILE__, __LINE__);
		while (std::getline(in, line))
		{
			if (isHTML)
			{
				ret += "\n";
				ret += line;
			}
			else
			{
				ret += "\n";
				ret += line;
			}
		}
		in.close();
	}
	_status = 200;
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

	url = str.substr(0, str.find_first_of('?'));
	if (!(*(url.rbegin()) == '/'))
		url.append("/");

	if (!(isPermit(_target_path) & READABLE) || !(isPermit(_target_path) & EXCUTABLE))
	{
		_status = _return == -1 ? 403 : _return;
		return "";
	}

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
			std::string path_to_index;
			if (!(*(_target_path.rbegin()) == '/'))
				path_to_index = _target_path + "/" + filename;
			else
				path_to_index = _target_path + filename;
			if (stat(path_to_index.c_str(), &fileinfo) == 0)
			{
				if (S_ISDIR(fileinfo.st_mode))
				{
					filename += "/";
					ret += "<a href=\"" + url + filename + "\">" + filename + "</a>";
				}
				else
					ret += "<a href=\"" + url + filename + "\">" + filename + "</a>";
			}
			else
			{
				perror("BIG ERROR"); // <-- COMMENT THIS OUT LATER
				continue ;
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
	_status = 200;
	return (ret);
}

std::string	Response::getExt(std::string const &filename) const
{
	std::string	ext;
	std::string::size_type	idx;
	idx = filename.rfind(".");
	if (idx != std::string::npos)
		ext = filename.substr(idx + 1);
	else
		ext = "default";
    return ext;
}

void	Response::setRequestVal(void)
{
	std::map<std::string, std::string> reqHead = _request.getHead();

	for (std::map<std::string, std::string>::iterator it = reqHead.begin(); it != reqHead.end(); ++it)
	{
		if (it != _headers.end() && it->first != "Content-Length" && _headers.find(it->first) != _headers.end())
 			_headers[it->first] = it->second;
	}     
}    


void	Response::setContentType(std::string ext)
{
	_headers["Content-Type"] = _mimeList.getMimeType(ext);
}

bool	Response::checkSetLocation(std::string path)
{
	std::pair<bool, Location> location_pair;

	location_pair = _server.srchLocation(path);
	if (location_pair.first == true)
	{
		ft_logger("Location successfully set", DEBUG, __FILE__, __LINE__);
		setLocation(location_pair.second);
		return (true);
	}
	else
		return (false);
}

void	Response::setStatus(int err)
{
	this->_status = err;
	this->_req_status = true;
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

	if (remove(const_cast<char*>(_target_path.c_str())) == -1)
		status = 204;
	else
		_body = "\r\nSuccessfully deleted: " + _target_path;
	return (status);
}

std::string		Response::buildHeader(int bodySize, int status)
{
	std::string	header;

	setContentLength(bodySize);
	header += makeStartLine(status);
	header += makeTimeLine();
	header += appendMapHeaders(status);
	
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


std::string		Response::appendMapHeaders(int statusCode)	
{
	std::string	headerStr;

	for (std::map<std::string, std::string>::iterator it=_headers.begin(); it!=_headers.end(); it++)
	{
		if ( !(it->second.empty()) )
		{
			if ((_request.getMethodEnum() == DELETE && it->first == "Content-Type") 
			|| (it->first == "Transfer-Encoding")) // || (it->first == "Content-Type")
			{
				ft_logger("skip : " + it->first, DEBUG, __FILE__, __LINE__);
				continue ;
			} 
			headerStr += it->first;
			headerStr += ": ";
			if (it->first == "Content-Type" && statusCode >= 400)
				headerStr += "text/html";
			else
				headerStr += it->second;
			headerStr += "\r\n";
		}
	}
	return headerStr;
}

// Date: Thu, 18 Aug 2022 11:02:41 GMT
std::string		Response::makeTimeLine() 
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
	if (_request.getMethodEnum() != DELETE)
		timeLine += "\r\n";
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

void Response::setLocationHeader(std::string url)
{
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
