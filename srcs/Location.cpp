#include "Location.hpp"

Location::Location( void ) {}

Location::~Location( void ) {}

Location::Location( std::string locationBlock )
{
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

void    Location::setAttributes(std::string key, std::string value)
{
    const int N = 7;
    std::string keys[N] = {"location", 
                          "root", 
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
        initRoot(value);
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
    this->_root = src._root;
    this->_index = src._index;
    this->_autoIndex = src._autoIndex;
    this->_ret = src._ret;
    this->_allowMethods = src._allowMethods;
    this->_cgi = src._cgi;
    this->_block = src._block;

}

Location& Location::operator=( const Location& src )
{
    if (this != &src)
    {
        this->_path = src._path;
        this->_root = src._root;
        this->_index = src._index;
        this->_autoIndex = src._autoIndex;
        this->_ret = src._ret;
        this->_allowMethods = src._allowMethods;
        this->_cgi = src._cgi;
        this->_block = src._block;
    }
	return (*this);
}

std::ostream& operator<<(std::ostream& os, const Location& location)
{
    os << "path: " << location._path << std::endl;
    os << "root: " << location._root << std::endl;
    os << "index: " << location._index << std::endl;
    os << "autoIndex: " << location._autoIndex << std::endl;
    os << "return: " << location._ret << std::endl;
    os << "allow_methods: [";
    for (size_t i = 0; i < location._allowMethods.size(); i++)
        os << location._allowMethods[i] << " ";
    os << "]\n";
    os << "cgi: [";
    for (std::map<std::string, std::string>::const_iterator it = location._cgi.begin(); it != location._cgi.end(); it++)
        os << it->first << ", " << it->second << " ";
    os << "]\n";
    // os << "block: " << location._block << std::endl;
    return os;
}

std::string Location::getBlock() const
{
    return (this->_block);
}

void Location::initPath(std::string value)
{
    this->_path = value;
}

void Location::initRoot(std::string value)
{
    this->_root = value;
}

void Location::initIndex(std::string value)
{
    this->_index = value;
}

void Location::initAutoIndex(std::string value)
{
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
    
    while (std::getline(ss, cgiPath, ',') && std::getline(ss, suffix, ','))
    {
        this->_cgi[cgiPath] = suffix;
    }
}

