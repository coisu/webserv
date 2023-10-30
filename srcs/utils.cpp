#include "utils.hpp"

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
	return (stat((path).c_str(), &statbuf) == 0);

}

// helper function to trim leading and trailing whitespace from a string
std::string trimWhiteSpace( const std::string &str )
{
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    if (start == std::string::npos || end == std::string::npos)
        return ("");
    else
        return (str.substr(start, end - start + 1));
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

int	pathIsDir(std::string path)
{
	struct stat	statbuf;
	int ret = UNDEFINE;

	if (stat(path.c_str(), &statbuf) != 0)
		return ( N_FOUND);
	else if (S_ISDIR(statbuf.st_mode))
		ret = IS_DIR;
	else if (S_ISREG(statbuf.st_mode))
		ret = IS_REG;
	// else{
	// 	if (!(statbuf.st_mode & S_IRUSR))	// read permissions
	// 		return (std::cerr << "no read permissions", N_PERMIT_READ);
	// 	if (!(statbuf.st_mode & S_IWUSR))	// write permissions
	// 		return (std::cerr << "no write permissions", N_PERMIT_WRITE);
	// 	if (!(statbuf.st_mode & S_IXUSR))	// exec permission
	// 		return (std::cerr << "no exec permissions", N_PERMIT_EXEC);
	// }
	return (ret);
}

int getpermit(std::string path)
{
	struct stat	statbuf;
	int ret = UNDEFINE;

	if (stat(path.c_str(), &statbuf) != 0)
		return (N_FOUND);
	else
	{
		if (!(statbuf.st_mode & S_IRUSR))	// read permissions
			return (std::cerr << "no read permissions", N_PERMIT_READ);
		if (!(statbuf.st_mode & S_IWUSR))	// write permissions
			return (std::cerr << "no write permissions", N_PERMIT_WRITE);
		if (!(statbuf.st_mode & S_IXUSR))	// exec permission
			return (std::cerr << "no exec permissions", N_PERMIT_EXEC);
	}
	retrun (ret);
}

bool isNumeric(std::string const &str)
{
    size_t i = 0;

    while (i < str.length() - 1 && std::isdigit(str.at(i))) {
        i++;
    }
    return !str.empty() && i == str.length() - 1;
}