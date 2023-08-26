#include "Location.hpp"

Location::Location( void ) {}

Location::~Location( void ) {}

Location::Location( std::string locationBlock )
{
    std::cout << "location block: " << locationBlock << std::endl;
}

Location::Location( const Location& src )
{
    *this = src;
}

Location& Location::operator=( const Location& src )
{
    if (this != &src)
    {}
	return (*this);
}