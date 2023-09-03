#include "utils.h"

// e_method	Request::extractMethodType(std::string info)
// {
// 	size_t		i = 0, n = 0;
// 	std::string methods[3] = {"GET", "POST", "DELETE"};
// 	std::string	type;

// 	while (!std::isspace(info[n]))
// 		n++;
// 	type = info.substr(0, n);
// 	while (!methods[i].empty() && methods[i] != type)
// 		i++;
// 	return ((t_method)i);
// }

// std::string	Request::extractURL(std::string info)
// {
// 	size_t	i, n = 0;

// 	i = info.find_first_of('/');
// 	while (!std::isspace(info[i + n]))
// 		n++;
// 	return (info.substr(i, n));
// }

bool	pathExists(std::string path)
{
	struct stat	statbuf;
	return (stat((temp_config.root + path).c_str(), &statbuf) == 0);

}

std::string	readFile(std::string location)
{
	std::ifstream t(location.c_str());
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::cout << "buff: " << buffer << "\nloc: " << location << std::endl;
	return (buffer.str());
}

std::vector<std::string>	splitUrl(std::string url)
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
	// for (long unsigned int i = 0; i < vec.size(); i++)
	// 	std::cout << "vec: " << vec[i] << std::endl;
	return (vec);
}

bool	pathIsDir(std::string path)
{
	struct stat	statbuf;

	if (stat(path.c_str(), &statbuf) == 0)
	{
        if (!S_ISDIR(statbuf.st_mode))
			return false;
		if (!(statbuf.st_mode & S_IRUSR) ||	// read permissions
			!(statbuf.st_mode & S_IWUSR) ||	// write permissions
			!(statbuf.st_mode & S_IXUSR))	// exec permission
			return false;
	}
    else
	{
    	throw std::runtime_error(std::string(strerror(errno)));
	}
	return true;
	// return (stat(path.c_str(), &statbuf) == 0 && S_ISDIR(statbuf.st_mode));
}
