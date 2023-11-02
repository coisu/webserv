#include "utils.hpp"

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
	ft_logger("buff: " + buffer.str() + "\nloc: " + location, DEBUG, __FILE__, __LINE__);
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

char isPermit(std::string path)
{
	struct stat	statbuf;
	char ret = 0;

	if (stat(path.c_str(), &statbuf) != 0)
		return (0);
	else
	{
		if (!(statbuf.st_mode & S_IRUSR))	// read permissions
			ret += 4;
		if (!(statbuf.st_mode & S_IWUSR))	// write permissions
			ret += 2;
		if (!(statbuf.st_mode & S_IXUSR))	// exec permission
			ret += 1;
	}
	return (ret);
}

bool isNumeric(std::string const &str)
{
    size_t i = 0;

    while (i < str.length() - 1 && std::isdigit(str.at(i))) {
        i++;
    }
    return !str.empty() && i == str.length() - 1;
}

void ft_logger(std::string thing, int level, std::string FILE, int LINE)
{
	// std::cerr << "LOG_LEVEL: " << LOG_LEVEL << std::endl;
	if (level > LOG_LEVEL)
		return ;
	std::cout << "[\033[1;34m" << FILE << ":" << LINE << "\033[0m]\t";
	if (level == 0)
		std::cout << "\033[1;31m" << thing << "\033[0m" << std::endl;
	else if (level == 1)
		std::cout << "\033[1;32m" << thing << "\033[0m" << std::endl;
	else if (level == 2)
		std::cout << "\033[1;33m" << thing << "\033[0m" << std::endl;
}

std::string toLowercase(const std::string& str) {
    std::string lowerStr = str;
    for (size_t i = 0; i < lowerStr.length(); ++i) {
        lowerStr[i] = static_cast<char>(tolower(static_cast<unsigned char>(lowerStr[i])));
    }
    return lowerStr;
}