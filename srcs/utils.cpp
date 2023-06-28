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

bool	fileExists(std::string path)
{
	struct stat		statbuf;
	return (stat((temp_config.root + path).c_str(), &statbuf) == 0);

}

// bool	extractDirStatus(std::string url)
// {
// 	struct stat		statbuf;

// 	return ((stat((temp_config.root + url).c_str(), &statbuf) == 0) ? (S_ISDIR(statbuf.st_mode)) : throw 404);
// 	// throw 404;
// 	// return (0);
// }

// std::string	CGI::extractPathInfo(std::vector<std::string> urlvec)
// {
// 	std::string	path_info;

// 	if (urlvec.size() > 2)
// 	{
// 		size_t i = 2;
// 		while (i < urlvec.size() && urlvec[i][0] != '?')
// 			path_info += urlvec[i++];
// 	}
// 	return (path_info);
// }

// std::string	CGI::extractQueryString(std::vector<std::string> urlvec)
// {
// 	std::string	query;

// 	for (size_t i = 0; i < urlvec.size(); i++)
// 	{
// 		if (urlvec[i][0] == '?')
// 			return (urlvec[i].substr(1));
// 	}
// 	return (query);
// }