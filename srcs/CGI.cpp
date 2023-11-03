#include "CGI.hpp"

CGI::CGI(const Server& serv, const Location& location, const Request& request) 
: server(serv), _location(location), _request(request), _cgiConfig(location.getCGIConfig())
{
	this->_env = constructEnv(request.getURL(), request.getMethodStr());
	// std::cout << "CGI created\n";
}

CGI::~CGI()
{
	// std::cout << "CGI destroyed\n";
}

CGI::CGI(const CGI& copy) : server(copy.server), _location(copy._location), _request(copy._request)
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

std::map<std::string, std::string>	CGI::constructEnv(std::string RequestUrl, std::string methodString)
{
	std::map<std::string, std::string>	env;
	std::map<std::string, std::string>::iterator it;
	std::vector<std::string>			urlvec = splitUrl(RequestUrl);

	env["SERVER_SOFTWARE"] = "Jisu-Yoel-Amanda-Softwre";
	env["SERVER_NAME"] = this->server.getServerName();
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_PORT"] = SSTR(this->server.getPort());
	env["REQUEST_METHOD"] = methodString;
	env["PATH_INFO"] = this->server.getRoot() + extractPathInfo(urlvec);
	env["QUERY_STRING"] = extractQueryString(urlvec);
	env["UPLOAD_STORE"] = this->_location.getUploadStore();
	env["SCRIPT_NAME"] = extractScriptName(urlvec);
	env["CONTENT_TYPE"] = this->_request.getHead()["content-type"];
	env["UPLOAD_DIR"] = this->server.getRoot() + this->_location.getUploadStore();
	ft_logger("UPLOAD_DIR: " + env["UPLOAD_DIR"], DEBUG, __FILE__, __LINE__);
	it = env.begin();
	while( it != env.end())
	{
		if (it->second.empty())
			env.erase(it++);
		else
			it++;
	}
	identifyCGI(urlvec);
	this->_av[0] = const_cast<char*>(this->_program.c_str());
	this->_av[1] = const_cast<char*>(this->_script.c_str());
	this->_av[2] = NULL;
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
	return (char_env);
}

void CGI::exec_cgi( int &read_fd, int &write_fd, int &cgi_pid )
{
	int child_to_parent[2];
	int parent_to_child[2];
	pid_t pid;
	char** const argv = this->_av;
	if (!argv)
	{
		ft_logger("Failed to run CGI: argv is empty", ERROR, __FILE__, __LINE__);
		throw std::runtime_error("Failed to run CGI: argv is empty");
	}
	const char* cgi_path = argv[0];
	char** const envp = this->getCharEnv();
	// std::string response;

	// Create a pipes
	if (pipe(child_to_parent) == -1)
	{
		perror("pipe"); // <-- COMMENT THIS OUT LATER
		ft_logger("Failed to create pipe", ERROR, __FILE__, __LINE__);
		throw std::runtime_error("Failed to create pipe");
	}
	if (pipe(parent_to_child) == -1)
	{
		perror("pipe"); // <-- COMMENT THIS OUT LATER
		ft_logger("Failed to create pipe", ERROR, __FILE__, __LINE__);
		throw std::runtime_error("Failed to create pipe");
	}

	// Fork the process
	pid = fork();
	if (pid == -1)
	{
		perror("fork"); // <-- COMMENT THIS OUT LATER
		ft_logger("Failed to fork process", ERROR, __FILE__, __LINE__);
		throw std::runtime_error("Failed to fork process");
	}

	if (pid == 0) // Child process
	{
		close(parent_to_child[1]); // Close the write end we don't need.
		close(child_to_parent[0]); // Close the read end we don't need.

		// Redirect stdout to the write end of the pipe
		dup2(parent_to_child[0], STDIN_FILENO); // Redirect read end to stdin.
		dup2(child_to_parent[1], STDOUT_FILENO); // Redirect stdout to write end.

		close(parent_to_child[0]); // These are no longer needed after dup2.
		close(child_to_parent[1]);

		// Execute the CGI program
		if (execve(cgi_path, argv, envp) == -1)
			perror("execve"); // <-- COMMENT THIS OUT LATER
		// close(STDIN_FILENO);
		// close(STDOUT_FILENO);
		_exit(EXIT_FAILURE);
	}
	else // Parent process
	{
		close(parent_to_child[0]); // Close the read end we don't need.
		close(child_to_parent[1]); // Close the write end we don't need.

		read_fd = child_to_parent[0]; // <-- Save the read end of the pipe for the multiplexer
		write_fd = parent_to_child[1]; // <-- Save the write end of the pipe for the multiplexer

		cgi_pid = pid; // <-- Save the CGI process ID
	}
	// free envp
	for (int i = 0; envp[i]; i++)
		delete[] envp[i];
	delete[] envp;
}

void	CGI::identifyCGI(std::vector<std::string> urlvec)
{
	std::string path, ext;
	std::string root = this->server.getRoot();
	for (size_t i = 0; i < urlvec.size() && urlvec[i][0] != '?'; i++)
		path += urlvec[i];	
	size_t pos = path.find_last_of('.');
	if (pos != std::string::npos)
		ext = path.substr(pos);
	if (ext.empty())
	{
		path = path + "/" + this->_location.getIndex();
		pos = path.find_last_of('.');
		if (pos != std::string::npos)
			ext = path.substr(pos);
	}
	this->_script = root + path;
	this->_postfix = ext;
	this->_program = this->_cgiConfig[this->_postfix];
	if (this->_program.empty())
	{
		ft_logger("Error: Not a CGI", ERROR, __FILE__, __LINE__);
		throw 403;
	}
}

std::string	CGI::extractScriptName(std::vector<std::string> urlvec)
{
	std::string	script_name;

	if (urlvec.size() >= 2)
		script_name = urlvec[0] + urlvec[1];
	return (script_name);
}

std::string	CGI::extractPathInfo(std::vector<std::string> urlvec)
{
	std::string	path_info;

	if (urlvec.size() > 2)
	{
		size_t i = 2;
		while (i < urlvec.size() && urlvec[i][0] != '?')
			path_info += urlvec[i++];
	}
	return (path_info);
}

std::string	CGI::extractQueryString(std::vector<std::string> urlvec)
{
	std::string	query;

	for (size_t i = 0; i < urlvec.size(); i++)
	{
		std::stringstream ss;
		ss << "Query string ===== " << urlvec[i] <<std::endl;
		ft_logger(ss.str(), DEBUG, __FILE__, __LINE__);
		if (urlvec[i][0] == '?')
			return (urlvec[i].substr(1));
	}
	return (query);
}