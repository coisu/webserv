
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
	std::cout << "============================\n";
	std::cout << responseBuffer << std::endl;
	std::cout << "============================\n";
	
}

Response::Response(const Request &request, Server &server ) : _request(request), _server(server)
{
	// _target_path = _server.getRoot() + "/";
	_target_path = _request.getLocPath();
	std::cerr << "target path: " << _target_path << std::endl;
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

std::string Response::processResponse()
{
	// std::string	currentMethod(_request.getMethodEnum());
	_currentMethod = _request.getMethodEnum();
	// std::string	ext(getExt(_request.getLocPath()));
	bool isRedirect = false;

	if (_currentMethod == DELETE && _status == 404)
		_status = -1;
	if (_request.getBody().length() > _server.getClientBodySize())
	{
		std::cout << "_request.getBody() : " << _request.getBody() << std::endl;
		std::cout << "clien max body size : " << _server.getClientBodySize() << std::endl << std::endl;
		setStatus(413);
	}
	setRequestVal();
	if(!checkSetLocation(_target_path))
	{
		Location L("location/;allow_methods:DELETE,POST,GET;autoindex:on;");
		std::cout << "Location set with default\n";
		setLocation(L);
	}

	// setTargetPath();



	if (_location.getAlias() != "")
	{
		std::string file;
		std::string uri = _request.getURL();
		std::cout <<  "request uri : " << uri << std::endl;
		file = uri.substr(uri.rfind("/"), uri.length());
		std::cout << "rfind index : " << uri.rfind("/") << ", length : " << uri.length() <<std::endl;
		std::cout << "derective file is : " << file <<std::endl ;
		_target_path = _location.getIndex() == "" ? _location.getAlias() + file : _location.getAlias() + file + _location.getIndex();
	}
	std::cout << "****target path : " << _target_path << "\n****method : " << _currentMethod << std::endl;

	if (_location.getIndex() != "" && \
		(pathExists(_target_path + _location.getIndex()) || (pathIsDir(_target_path) == IS_DIR && _currentMethod == POST)))
	{
		std::cout << "\nSUCCEED!!!!\n\n";
		_target_path += _location.getIndex();
	}
	else if (_location.getIndex() == "" && _location.getRet() == "" \
			 && !_location.getIsCGI() && pathExists(_target_path + "index.html") \
			 && _currentMethod != POST)
	{
		_target_path += "index.html";
	}
	if (!_location.getAutoIndex() && _location.getRet() == "")
	{
		std::cout << "**[TARGET_FILE] : " << _target_path << std::endl;
		int ret = pathIsDir(_target_path);
		if (ret == IS_DIR)
		{
			if (_currentMethod == POST)
				setStatus(405);
			else
				setStatus(403);
		}
		else

		if (!pathExists(_target_path) && _currentMethod != POST)
		{
			setStatus(404);
		}
	}
	std::string ext = getExt(_target_path);
	if (_headers["Content-Type"] == "")
		setContentType(ext); 
	std::cout << "\n\n--------------<<<<<<<< INFO CHECK >>>>>>>>--------------\n" << std::endl;
	std::cout << "LOC   [PATH] : " << _location.getPath() << std::endl;
	std::cout << "LOC [RETURN] : " << _location.getRet() << std::endl;
	std::cout << "LOC  [INDEX] : " << _location.getIndex() << std::endl;
	std::cout << "    [TARGET] : " << _target_path << std::endl;
	std::cout << "    [E-CODE] : " << _status << std::endl;
	std::cout << "\n-------------->>>>>>>> INFO CHECK <<<<<<<<--------------\n" << std::endl;

	if (!_location.getRet().empty())
	{
		int	code = 0;
		std::string str = "";
		
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
			setStatus(code);
		if (_status == 301 || _status == 302 || _status == 303 || _status == 307 || _status == 308)
		{
			if (!str.empty())
				setLocationHeader(str);
			else
				setLocationHeader(" ");
		}
		if (_status == -1 && _req_status == false && !isRedirect)
		{
			_status = 301;	//MOVED_PERMANENTLY
			isRedirect = true;
		}
		if (isRedirect && _location.getIndex() == "")
		{
			int i;
			std::cout << "rfound : " << _target_path.rfind("/")<< std::endl;
			std::cout << "length : " <<  _target_path.length()<< std::endl;
			if (_target_path.rfind("/") == _target_path.length() - 1)
			{
				i = _target_path.rfind("/", _target_path.length() -2);
			}
			else
				i = _target_path.rfind("/", _target_path.length() - 1);
			std::cout << "FIND index : "<< i << std::endl;
			_target_path = _target_path.substr(0, i + 1);
			_target_path += "index.html";
		}
		std::cout << "[TARGET] : " << _target_path << std::endl;
		std::cout << "[E-CODE] : " << _status << std::endl;
	}

	if (_location.getIndex() != "" && ext != getExt(_location.getIndex()))
	{
		_status = 404;
	}
	if (isAllowedMethod(_currentMethod) && (_status == -1 || _status == 302))
		buildBodywithMethod(ext);
	if (_status >= 400)
		buildErrorBody(ext);	

	/* REDIRECTION HEADER */
	if ((!_location.getRet().empty() && isRedirect && _req_status == false) || _status == 201)
	{
		if (_headers["Location"] == "")
			setLocationHeader(_request.getURL());
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

	/* MAKE HEADER */
	if (((_currentMethod == GET || _currentMethod == POST) && ext != "php") || _status >= 400)
		_headerStr += buildHeader(_body.size(), _status);
	else
		_headerStr += buildHeaderCgi(_body, _status);

	_buffer = (_body == "") ? _headerStr + "\r\n\r\n" : _headerStr + "\r\n" + _body + "\r\n";


	std::cout << "__________________RESPONSE___________________\n" << _buffer << "\n______________________________________________\n";
	return _buffer;
}

void Response::setTargetPath()
{
	// _Target_path = locPath
	/* CASE: alias O */
	std::string uri = _request.getURL();
	if (_location.getAlias() != "")		// manage casese Alias Exist
	{
		std::string resource;
		unsigned int idx = uri.find(_location.getPath());
		if (idx != std::string::npos)
			idx += _location.getPath().length();
		resource = uri.substr(idx, uri.length());
		std::cout << "derective addr is : " << resource <<std::endl ;
		if (_location.getIndex() == "")
			_target_path = _location.getAlias() + resource;
		else							// Alias with index
		{
			int aliasRes = pathIsDir(_location.getAlias() + resource);
			if (aliasRes == IS_DIR || (aliasRes == N_FOUND))
			{
				_target_path = _location.getAlias() + _location.getIndex();
			}
			else
			{
				_target_path = _target_path = _location.getAlias() + resource;
				if (pathExists(_location.getAlias() + _location.getIndex()))
				{
					_target_path = _location.getAlias() + _location.getIndex();
				}
			}
		}
		std::cout << "****target path : " << _target_path << "\n****method : " << _currentMethod << std::endl;
	}
	else 
	{
		/* CASE: alias X, index O */
		if (pathIsDir(uri) == IS_REG || ())
		if (_location.getIndex() != "")
		{
			if (pathIsDir(_target_path) == IS_DIR)
			{
				if (pathExists(_target_path + _location.getIndex()) 
					|| _currentMethod == POST)
			}
		}
		/* CASE: alias X, index X */
		else 
		{

		}
	}
	/* handle return */
}

void Response::buildBodywithMethod(std::string ext)
{

	if (_currentMethod == GET || _currentMethod == POST)
	{
		if (_currentMethod == GET && ext != "php")	//html
		{
			if (_location.getAutoIndex())
			{

				int ret = pathIsDir(_target_path);

				std::cout << " &&& Target Path : " << _target_path << std::endl;
				if (ret == N_FOUND)
					_status = 404;
				else if (ret == IS_DIR)
				{
					std::cout << "\n\n... target file is directory ...\n\n";
					_body = writeBodyAutoindex(_request.getURL());
					_status = 200;
				}
				else if (ret == IS_REG)						//regular file
				{
					std::cout << "\n\n... target file is regular file ...\n\n";
					_body = fileTextIntoBody(_mimeList.getMimeType(ext) == "text/html");
					if (_status == -1)
						_status = 200;
				}
				else
				{
					std::cout << "STAT : " << ret <<std::endl;
					_status = 403;
				}
			}
			else
			{
				std::cout << "\n\n... MANUAL BODY WRITER ...\n\n";

				std::pair<bool, std::string> body_pair;
				body_pair = writeBodyHtmlPair(_target_path, _mimeList.getMimeType(ext) == "text/html");
				if (body_pair.first == true)
				{
					_status = 200;
					_body = body_pair.second;
				}
				else
				{
					_status = 404;
					_body = body_pair.second;
				}
			}

		}
		else
		{
			// CGI	cgi(_server, _request.getURL(), _request.getMethodStr(), _location.getCGIConfig());
			// _body = cgi.exec_cgi();
			// std::cout << "\n\n>> CGI BODY PRINT >>>>>>>>>>\n";
			// std::cout << _body;
			// std::cout << "\n<<<<<<<<<<<<<<<<<<CGI BODY PRINT\n\n";
			if (_currentMethod == POST)
			{
				int ret = pathIsDir(_target_path);
				if (ret == IS_REG || ret == N_FOUND)
				{
					std::string reqBody = _request.getBody();
					std::cout << "\n   request body : " << reqBody << std::endl;
					_body = reqBody;
					// if (ext == "html" && reqBody.find("&") != std::string::npos)
					// 	_headers["Content-Type"] = "application/x-www-form-urlencoded";
					std::cout << "Content-type : " << _headers["Content-Type"] <<std::endl;
					int	fd = open(_target_path.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0665);
					if (fd > 0 && reqBody.length() && write(fd, reqBody.c_str(), reqBody.length()) > 0)
					{
						std::cout << "Created by POST at " << _target_path <<std::endl;
					}
					close(fd);
					_status = (ret == N_FOUND) ? 201 : 200;
				}
				else
				{
					std::cout << "POST creating failed : stat : " << ret <<std::endl;
					if (ret == IS_DIR)
						_status = 405;
					else
						_status = 403;
					// _body = _request.getBody();
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

void Response::buildErrorBody(std::string ext)
{

	if (_status >= 400)
	{
		std::cout << "status -- " << _status << std::endl;
		std::map<int, std::string> ep = _server.getErrorPages();
		if (ep.find(_status) != ep.end())
			_body = writeBodyHtml(_server.getRoot() + ep[_status], _mimeList.getMimeType(ext) == "text/html");
		else
			_body = makeErrorPage(_status);

		_connect = "Close";
	}

}

std::pair<bool, std::string>		Response::writeBodyHtmlPair(std::string filePath, bool isHTML)
{
	std::string		ret;
	std::ifstream 	ifs;
	
	// if (path[0] != '/')
	// 	filePath = "/" + path;
	ifs.open(const_cast<char*>(filePath.c_str()));
	
	if (ifs.fail())
	{
		ifs.close();
		return (std::make_pair(false, makeErrorPage(404)));
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
	ifs.close();

	return (std::make_pair(true, ret));
}

std::string		Response::writeBodyHtml(std::string filePath, bool isHTML)
{
	std::string		ret;
	std::ifstream 	ifs;
	
	// if (path[0] != '/')
	// 	filePath = "/" + path;
std::cout << "root : " <<_server.getRoot() << std::endl;
std::cout<< "__filePath in writeBodyHtml : " << filePath << std::endl;
	ifs.open(const_cast<char*>(filePath.c_str()));
	
	if (ifs.fail())
	{
		ifs.close();
		std::map<int, std::string> ep = _server.getErrorPages();
		ifs.open(const_cast<char*>((_server.getRoot() + ep[404]).c_str()));
		if (ifs.fail())
		{
			ifs.close();
			return makeErrorPage(404);
		}
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
	ifs.close();

	return (ret);
}

std::string		Response::fileTextIntoBody(bool isHTML)
{
	std::ifstream in(_target_path.c_str());
	std::string line;
	std::string ret;

	if (in.is_open())
	{
		std::cout << "file opened\n\n";
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
				perror("BIG ERROR");
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
    return ext;
}

void	Response::setRequestVal(void)
{
	std::map<std::string, std::string> reqHead = _request.getHead();
	// std::map<std::string, std::string>::iterator it = reqHead.begin();


	// 	if (it->first != "Content-Length" && _headers.find(it->first) != _headers.end())
	// 	{
	// 		std::cout <<"IN?????" <<std::endl;
	// 		_headers[it->first] = it->second;
	// 		std::cout << "[ " << it->first << " ] : " << it->second << std::endl;
	// 	}
	// std::map<std::string, std::string> Header = _request.getHead();

	std::cout << "===========HERE==================" << std::endl;
	for (std::map<std::string, std::string>::iterator it = reqHead.begin(); it != reqHead.end(); ++it)
	{
		// Header = _headers.find(it->first);
		if (it != _headers.end() && it->first != "Content-Length" && _headers.find(it->first) != _headers.end())
		{
 			_headers[it->first] = it->second;
			std::cout << "[ " << it->first << " ] : " << it->second << std::endl;
		}
	}     
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
		std::cout << "Location successfully set\n";
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
	{
		status = 204;
	}
	else
	{
		_body = "\r\nSuccessfully deleted: " + _target_path;
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

	if (_request.getMethodEnum() == DELETE && _status == 204)
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
				std::cout << "\n\nisCGI : " << isCGI <<std::endl;
				std::cout << "skip : " <<it->first <<std::endl<<std::endl;
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
	if (isCGI == false || _request.getMethodEnum() != DELETE)
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
