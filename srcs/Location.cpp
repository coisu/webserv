#include "Location.hpp"

Location::Location( void ) : _isCgi(false) {}

Location::~Location( void ) {}

Location::Location( std::string locationBlock ) : _isCgi(false)
{
	initDefaults();
    std::stringstream   ss(locationBlock);
    std::string         part;

    this->_block = locationBlock;
    std::getline(ss, part, ';');
    setAttributes(part.substr(0, part.find('/')), part.substr(part.find('/')));
    while (std::getline(ss, part, ';'))
    {
        std::string key = part.substr(0, part.find(':'));
        std::string value = part.substr(part.find(':')+1);
        // std::cout << "key: \"" << key << "\" value: \"" << value << "\"\n";
        setAttributes(key, value);
    }
    // std::cout << "SERVER:\n" << *this << std::endl;
}

void	Location::initDefaults()
{
	this->_path = "";
	// this->_root = "";
	this->_index = "";
	this->_autoIndex = false;
	this->_ret = "";
	// this->_allowMethids 
	// this->_cgi
	this->_block = "";
	this->_isCgi = false;
}

void    Location::setAttributes(std::string key, std::string value)
{
    const int N = 7;
    std::string keys[N] = {"location", 
                          "alias", 
                          "index", 
                          "autoindex",
                          "return",
                          "allow_methods",
                          "cgi"};
    size_t i = 0;
    while (i < N && keys[i] != key)
        i++;
    switch (i)
    {
    case 0:
        initPath(value);
        break;
    case 1:
        initAlias(value);
        break;
    case 2:
        initIndex(value);
        break;
    case 3:
        initAutoIndex(value);
        break;
    case 4:
        initRet(value);
        break;
    case 5:
        initAllowMethods(value);
        break;
    case 6:
        initCGI(value);
        break;
    default:
        throw std::runtime_error("unrecognised location key: " + key);
    }
}

Location::Location( const Location& src )
{
    this->_path = src._path;
    this->_alias = src._alias;
    this->_index = src._index;
    this->_autoIndex = src._autoIndex;
    this->_ret = src._ret;
    this->_allowMethods = src._allowMethods;
    this->_cgiConfig = src._cgiConfig;
    this->_block = src._block;
	this->_isCgi = src._isCgi;
}

Location& Location::operator=( const Location& src )
{
    if (this != &src)
    {
        this->_path = src._path;
        this->_alias = src._alias;
        this->_index = src._index;
        this->_autoIndex = src._autoIndex;
        this->_ret = src._ret;
        this->_allowMethods = src._allowMethods;
        this->_cgiConfig = src._cgiConfig;
        this->_block = src._block;
		this->_isCgi = src._isCgi;
    }
	return (*this);
}

std::ostream& operator<<(std::ostream& os, const Location& location)
{
    os << "path: " << location._path << std::endl;
    os << "alias: " << location._alias << std::endl;
    os << "index: " << location._index << std::endl;
    os << "autoIndex: " << location._autoIndex << std::endl;
    os << "return: " << location._ret << std::endl;
    os << "allow_methods: [";
    for (size_t i = 0; i < location._allowMethods.size(); i++)
        os << location._allowMethods[i] << " ";
    os << "]\n";
    os << "cgi: [";
    for (std::map<std::string, std::string>::const_iterator it = location._cgiConfig.begin(); it != location._cgiConfig.end(); it++)
        os << it->first << ", " << it->second << " ";
    os << "]\n";
    // os << "block: " << location._block << std::endl;
    return os;
}

void Location::initPath(std::string value)
{
    if (value.size() < 1 || value[0] != '/')
        throw std::runtime_error("location routes must start with \'/\' - route: " + value);
    if (value.at(value.length() - 1) == '/' && value.size() > 1)
        throw std::runtime_error("location route must *not* end with \'/\' - route: " + value);
    this->_path = value;
}

void Location::initAlias(std::string value)
{
    if (value[0] != '/')
        throw std::runtime_error("alias path must start with \'/\' - path: " + value);
    if (value.at(value.length() - 1) == '/' && value.size() > 1)
        throw std::runtime_error("alias path must *not* end with \'/\' - path: " + value);
    this->_alias = value;
}

void Location::initIndex(std::string value)
{
    this->_index = value;
}

void Location::initAutoIndex(std::string value)
{
    if (value != "on" && value != "off")
        throw std::runtime_error("autoindex must be \"on\" or \"off\".");
    this->_autoIndex = (value == "on") ? true : false;
}

void Location::initRet(std::string value)
{
    this->_ret = value;
}

void Location::initAllowMethods(std::string value)
{
    std::stringstream   ss(value);
    std::string         method;
    std::string methods[3] = {"GET", "POST", "DELETE"};

    while (std::getline(ss, method, ','))
    {
        int i = 0;
        while(i < 3 && method != methods[i])
            i++;
        switch (i)
        {
        case 0:
            this->_allowMethods.push_back(GET);
            break;
        case 1:
            this->_allowMethods.push_back(POST);
            break;
        case 2:
            this->_allowMethods.push_back(DELETE);
            break;
        default:
            throw std::runtime_error("unrecognised/unsupported method: " + method);
        }
    }
}

void Location::initCGI(std::string value)
{
    std::stringstream   ss(value);
    std::string         cgiPath;
    std::string         suffix;
    
	this->_isCgi = true;
    while (std::getline(ss, cgiPath, ',') && std::getline(ss, suffix, ','))
    {
        this->_cgiConfig[cgiPath] = suffix;
    }
}

std::string Location::getBlock() const
{
    return (this->_block);
}

std::string	Location::getPath() const
{
	return (this->_path);
}

bool	Location::getIsCGI() const
{
	return (this->_isCgi);
}

std::map<std::string, std::string>	Location::getCGIConfig() const
{
	return (this->_cgiConfig);
}

