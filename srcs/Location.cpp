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
    // *this = src;
    this->_block = src.getBlock();
}

Location& Location::operator=( const Location& src )
{
    if (this != &src)
    {}
	return (*this);
}

std::string Location::getBlock() const
{
    return (this->_block);
}