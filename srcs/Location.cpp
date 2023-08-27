#include "Location.hpp"

Location::Location( void ) {}

Location::~Location( void ) {}

Location::Location( std::string locationBlock )
{
    this->_block = locationBlock;
    // std::cout << "location block: " << locationBlock << std::endl;
}

Location::Location( const Location& src )
{
    this->_path = src._path;
    this->_root = src._root;
    this->_index = src._index;
    this->_retrn = src._retrn;
    this->_allow_method = src._allow_method;
    this->_cgi_path = src._cgi_path;
    this->_cgi_ext = src._cgi_ext;
    this->_block = src._block;
}

Location& Location::operator=( const Location& src )
{
    if (this != &src)
    {
        this->_path = src._path;
        this->_root = src._root;
        this->_index = src._index;
        this->_retrn = src._retrn;
        this->_allow_method = src._allow_method;
        this->_cgi_path = src._cgi_path;
        this->_cgi_ext = src._cgi_ext;
        this->_block = src._block;
    }
	return (*this);
}

std::ostream& operator<<(std::ostream& os, const Location& location)
{
    os << "path: " << location._path << std::endl;
    os << "root: " << location._root << std::endl;
    os << "index: " << location._index << std::endl;
    os << "retrn: " << location._retrn << std::endl;
    for (size_t i = 0; i < location._allow_method.size(); i++)
        os << "allow_method: " << location._allow_method[i] << std::endl;
    for (size_t i = 0; i < location._cgi_path.size(); i++)
        os << "cgi_path: " << location._cgi_path[i] << std::endl;
    for (size_t i = 0; i < location._cgi_ext.size(); i++)
        os << "cgi_ext: " << location._cgi_ext[i] << std::endl;
    os << "block: " << location._block << std::endl;
    return os;
}

std::string Location::getBlock() const
{
    return (this->_block);
}