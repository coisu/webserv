#include "CGI.hpp"

CGI::CGI(Server& serv, Location& location, Request& request) 
: server(serv), _location(location), _request(request), _cgiConfig(location.getCGIConfig())
{
	this->_env = constructEnv(request.getURL(), request.getMethodStr());
	// std::cout << "CGI created\n";
}
// : server(serv), _cgiConfig(cgiConfig) // : _env(construct_env(request))
// {
// 	this->_env = constructEnv(RequestUrl, methodString);
// 	// std::cout << "CGI created\n";
// }

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
	std::vector<std::string>			urlvec = splitUrl(RequestUrl);

	env["SERVER_SOFTWARE"] = "Jisu-Yoel-Amanda-Softwre";
	env["SERVER_NAME"] = this->server.getServerName();
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_PORT"] = SSTR(this->server.getPort());
	env["REQUEST_METHOD"] = methodString;
	env["PATH_INFO"] = this->server.getRoot() + extractPathInfo(urlvec);
	// if (!env["PATH_INFO"].empty())
		// env["PATH_TRANSLATED"] = temp_config.root + env["PATH_INFO"];
	// env["SCRIPT_NAME"] = temp_config.root + extractScriptName(urlvec);
	env["QUERY_STRING"] = extractQueryString(urlvec);
	// env["REMOTE_HOST"] = "";
	// env["REMOTE_ADDR"] = "";
	// env["AUTH_TYPE"] = "";
	// env["REMOTE_IDENT"] = "";
	// env["CONTENT_TYPE"] = "";
	// env["CONTENT_LENGTH"] = "";
	// env["HTTP_ACCEPT"] = "";
	// env["HTTP_ACCEPT_LANGUAGE"] = "";
	// env["HTTP_USER_AGENT"] = "";
	// env["HTTP_COOKIE"] = "";
	std::map<std::string, std::string>::iterator it = env.begin();
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
	this->_av[3] = NULL;
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
	// std::cout << "\n\n-------ENV-------\n" << std::endl;
	// i = 0;
	// for (std::map<std::string, std::string>::iterator it = this->_env.begin(); it != this->_env.end(); it++)
	// {
	// 	std::cout << char_env[i] << std::endl;
	// 	i++;
	// }
	// std::cout << "\n\n-----------------\n" << std::endl;
	return (char_env);
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

std::string CGI::exec_cgi( void )
{
	int pipefd[2];
	pid_t pid;
	char** const argv = this->_av;
	if (!argv)
		throw std::runtime_error("Failed to run CGI: argv is empty");
	const char* cgi_path = argv[0];
	char** const envp = this->getCharEnv();
	std::string response;

	// Create a pipe
	if (pipe(pipefd) == -1)
	{
		perror("pipe"); // <-- COMMENT THIS OUT LATER
		throw std::runtime_error("Failed to create pipe");
		throw std::runtime_error("Failed to create pipe");
	}

	// Fork the process
	pid = fork();
	if (pid == -1)
	{
		perror("fork"); // <-- COMMENT THIS OUT LATER
		throw std::runtime_error("Failed to fork process");
	}

	if (pid == 0)
	{
		// Child process

		// Redirect stdout to the write end of the pipe
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);

		// Execute the CGI program
		if (execve(cgi_path, argv, envp) == -1)
		{
			perror("execve"); // <-- COMMENT THIS OUT LATER
		}
		exit(1);
	}
	else
	{
		// Parent process

		// Close write end in parent
		close(pipefd[1]);

		char buffer[1024];
		ssize_t bytesRead;
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
		{
			// Here, buffer contains bytesRead bytes of output from the CGI program.
			// Send this back as the HTTP response.
			response += buffer;
			// write()
			// write(STDOUT_FILENO, buffer, bytesRead);
		}

		close(pipefd[0]);
		waitpid(pid, NULL, 0);
	}
	return (response);
}

// std::string	CGI::exec_cgi( void )
// {
// 	const char*	cmd;
// 	std::string	strcmd;
// 	char		buffer[128];
// 	std::string	envline;

// 	for (std::map<std::string, std::string>::iterator it = this->_env.begin(); it != this->_env.end(); it++)
// 		envline += (it->first + "=\'" + it->second + "\' ");
// 	strcmd = "echo \"" + this->_env["QUERY_STRING"] + "\" hello |" + envline + this->_program + " " + this->_script;
// 	cmd = strcmd.c_str();
// 	// std::cout << strcmd << std::endl;
// 	std::string result = "";
// 	FILE* pipe = popen(cmd, "r");
// 	if (!pipe)
// 		throw std::runtime_error("popen() failed!");
// 	try
// 	{
// 		while (fgets(buffer, sizeof buffer, pipe) != NULL)
// 			result += buffer;
// 	}
// 	catch (...)
// 	{
// 		pclose(pipe);
// 		throw ;
// 	}
// 	pclose(pipe);
// 	return result;
// }

// std::string	CGI::exec_cgi( void )
// {
// 	// char	buffer[1024];
// 	int	pipe_in[2];
// 	int	pipe_out[2];
// 	int	pid;
// 	int	exit_status;

// 	if (pipe(pipe_in) < 0)
// 		std::cerr << "pipe() failed" << std::endl, throw 500;
// 	if (pipe(pipe_out) < 0)
// 	{
// 		std::cerr << "pipe() failed" << std::endl;
// 		close(pipe_in[0]), close(pipe_in[1]), throw 500;
// 	}
// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		dup2(pipe_in[0], STDIN_FILENO);
// 		dup2(pipe_out[1], STDOUT_FILENO);
// 		close(pipe_in[0]);
// 		close(pipe_in[1]);
// 		close(pipe_out[0]);
// 		close(pipe_out[1]);
// 		exit_status = execve(this->_av[0], this->_av, this->getCharEnv());
// 		exit(exit_status);
// 	}
// 	else if (pid < 0)
// 		std::cout << "Fork failed" << std::endl, throw 500;
// 	return (res);
// }

void	CGI::identifyCGI(std::vector<std::string> urlvec)
{
	// std::cout << "URLVEC:\n";
	// for (size_t i = 0; i < urlvec.size(); i++)
	// 	std::cout << urlvec[i] << ", ";
	// std::cout << "end\n";
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
	// this->_program = this->_cgiConfig[this->_postfix];

	// for (size_t i = 0; i < urlvec.size(); i++)
	// {
	// 	for (std::map<std::string, std::string>::iterator it = this->_cgiConfig.begin(); 
	// 	it != this->_cgiConfig.end(); it++)
	// 	{
	// 		if (urlvec[i].find(it->first) == urlvec[i].size() - 3)
	// 			this->_script = root + extractPathInfo(urlvec), this->_postfix = it->first;
	// 			// this->_script = this->server.getRoot() + temp_config.cgi_folder + urlvec[i], this->_postfix = it->first;
	// 	}
	// }
	// // _script = "/workspaces/webserv/cgi-bin/";
	// // _script = "/workspaces/webserv/post/post.php";
	// if (this->_script.empty())
	// 	throw 501;
	// this->_program = this->_cgiConfig[this->_postfix];
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
		std::cout << "Query string ===== " << urlvec[i] <<std::endl;
		if (urlvec[i][0] == '?')
			return (urlvec[i].substr(1));
	}
	return (query);
}