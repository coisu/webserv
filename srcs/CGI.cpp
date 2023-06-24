#include "CGI.hpp"

CGI::CGI(Request& request)// : _env(construct_env(request))
{
	_env = construct_env(request);
	std::cout << "CGI created\n";
}

CGI::~CGI()
{
	// std::cout << "CGI destroyed\n";
}

CGI::CGI(const CGI& copy)
{
	std::cout << "CGI is being copied\n";
	*this = copy;
}

CGI&	CGI::operator = (const CGI& copy)
{
	std::cout << "CGI is being assigned\n";
	if (this != &copy)
	{
		// this->_body = copy.getBody().copy()
	}
	return (*this);
}

std::string	extractScriptName(std::vector<std::string> urlvec)
{
	std::string	script_name;
	
	if (urlvec.size() >= 2)
		script_name = urlvec[0] + urlvec[1];
	return (script_name);
}

std::vector<std::string>	split_url(std::string url)
{
	std::vector<std::string>	vec;

	std::size_t prev = 0, pos;
	while ((pos = url.find_first_of("/?", prev)) != std::string::npos)
	{
		if (pos > prev && prev > 0)
			vec.push_back(url.substr(prev - 1, pos - prev + 1));
		prev = pos + 1;
	}
	if (prev < url.length() && prev > 0)
		vec.push_back(url.substr(prev - 1, std::string::npos));
	// std::cout << "\n-----VEC-----\n";
	for (long unsigned int i = 0; i < vec.size(); i++)
		std::cout << "vec: " << vec[i] << std::endl;
	return (vec);
}

std::map<std::string, std::string>	CGI::construct_env(Request& request)
{
	(void)request;
	std::map<std::string, std::string>	env;
	std::string	url = request.getURL();
	std::vector<std::string>	urlvec = split_url(url);

	env["SERVER_SOFTWARE"] = "Jisu-Yoel-Amanda-Softwre";
	env["SERVER_NAME"] = temp_config.name;
	env["GATEWAY_INTERFACE"] = "CGI/1\\.1";
	env["SERVER_PROTOCOL"] = "HTTP/1\\.1";
	env["SERVER_PORT"] = SSTR(temp_config.host_port);
	env["REQUEST_METHOD"] = request.getMethodStr();
	env["PATH_INFO"] = extractPathInfo(urlvec);
	if (!env["PATH_INFO"].empty())
		env["PATH_TRANSLATED"] = temp_config.root + env["PATH_INFO"];
	env["SCRIPT_NAME"] = extractScriptName(urlvec);
	this->_program = temp_config.root + env["SCRIPT_NAME"];
	env["QUERY_STRING"] = extractQueryString(urlvec);
	env["REMOTE_HOST"] = "";
	env["REMOTE_ADDR"] = "";
	env["AUTH_TYPE"] = "";
	env["REMOTE_IDENT"] = "";
	env["CONTENT_TYPE"] = "";
	env["CONTENT_LENGTH"] = "";
	env["HTTP_ACCEPT"] = "";
	env["HTTP_ACCEPT_LANGUAGE"] = "";
	env["HTTP_USER_AGENT"] = "";
	env["HTTP_COOKIE"] = "";
	std::map<std::string, std::string>::iterator it = env.begin();
	while( it != env.end())
	{
		if (it->second.empty())
			env.erase(it++);
		else
			it++;
	}
	return (env);
}

char**	CGI::getCharEnv( void )
{
	char**	char_env = new char*[this->_env.size() + 1];
	int		i = 0;

	for (std::map<std::string, std::string>::iterator it = this->_env.begin(); it != this->_env.end(); it++, i++)
	{
		std::string elem = it->first + "=" + it->second;
		char_env[i] = new char[elem.size() + 1];
		strcpy(char_env[i], elem.c_str());
	}
	char_env[i] = NULL;
	std::cout << "\n\n-------ENV-------\n" << std::endl;
	i = 0;
	for (std::map<std::string, std::string>::iterator it = this->_env.begin(); it != this->_env.end(); it++)
	{
		std::cout << char_env[i] << std::endl;
		i++;
	}
	std::cout << "\n\n-----------------\n" << std::endl;
	return (char_env);
}

std::string	CGI::exec_cgi( void )
{
	const char*	cmd;
	std::string	strcmd;
	char		buffer[128];
	std::string	envline;

	for (std::map<std::string, std::string>::iterator it = this->_env.begin(); it != this->_env.end(); it++)
		envline += (it->first + "=" + it->second + " ");
	strcmd = envline + this->_program;
	cmd = strcmd.c_str();
	std::cout << strcmd << std::endl;
	std::string result = "";
	FILE* pipe = popen(cmd, "r");
	if (!pipe)
		throw std::runtime_error("popen() failed!");
	try
	{
		while (fgets(buffer, sizeof buffer, pipe) != NULL)
			result += buffer;
	}
	catch (...)
	{
		pclose(pipe);
		throw ;
	}
	pclose(pipe);
	return result;
}
