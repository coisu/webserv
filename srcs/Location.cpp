#include "Location.hpp"

Location::Location( void ) {}

Location::~Location( void ) {}

Location::Location( std::string locationBlock )
{
    (void)locationBlock;

}

Location::Location( Location& src )
{
    *this = src;

}

Location& Location::operator=( const Location& src )
{
    return(src);

}