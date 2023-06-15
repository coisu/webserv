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
	switch (i)
	{
		case 0:
			return (GET);
		case 1:
			return (POST);
		case 2:
			return (DELETE);
		default:
			return (UNDEFINED);
	}
}

std::string	extractURL(std::string info)
{
	size_t	i, n = 0;

	i = info.find_first_of('/');
	while (!std::isspace(info[i + n]))
		n++;
	return (info.substr(i, n));
}
