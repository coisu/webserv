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

std::string Location::getBlock() const
{
    return (this->_block);
}