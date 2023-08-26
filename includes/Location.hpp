#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <string>

class Location
{
    public:
        /*OCCF*/
        Location( void );
        ~Location( void );
        Location( std::string locationBlock );
        Location( Location& src );
        Location& operator=( const Location& src );
    /*attributes*/
    /*methods*/
    //getters
    //setters
    private:
    //attributes
    //methods

};

#endif