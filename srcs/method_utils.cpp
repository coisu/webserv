#include "utils.h"

e_method	extractMethodType(std::string info)
{
	size_t		i = 0, n = 0;
	std::string methods[3] = {"GET", "POST", "DELETE"};
	std::string	type;

	while (!std::isspace(info[n]))
		n++;
	type = info.substr(0, n);
	while (!methods[i].empty() && methods[i] != type)
		i++;
	// switch (i)
	// {
	// 	case 0:
	// 		return (GET);
	// 	case 1:
	// 		return (POST);
	// 	case 2:
	// 		return (DELETE);
	// 	default:
	// 		return (UNDEFINED);
	// }
	return ((t_method)i);
}

std::string	extractURL(std::string info)
{
	size_t	i, n = 0;

	i = info.find_first_of('/');
	while (!std::isspace(info[i + n]))
		n++;
	// if (n > 0 && info[i + n - 1] == '/')
	// 	n--;
	return (info.substr(i, n));
}

bool	extractDirStatus(std::string url)
{
	struct stat		statbuf;

	if (stat((temp_config.root + url).c_str(), &statbuf) == 0)
		return (S_ISDIR(statbuf.st_mode));
	perror("stat");
	return (0);
}

std::string	extractPathInfo(std::vector<std::string> urlvec)
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

std::string	extractQueryString(std::vector<std::string> urlvec)
{
	std::string	query;

	for (size_t i = 0; i < urlvec.size(); i++)
	{
		if (urlvec[i][0] == '?')
			return (urlvec[i].substr(1));
	}
	return (query);
}