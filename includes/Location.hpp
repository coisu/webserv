#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <string>
#include <vector>

class Location
{
    public:
        /*OCCF*/
        Location( void );
        ~Location( void );
        Location( std::string locationBlock );
        Location( const Location& src );
        Location& operator=( const Location& src );

    /*attributes*/

    /*methods*/
        //getters
        std::string getBlock() const;
        //setters
    private:
    /*attributes*/
        std::string                 _path;
        std::string                 _root;
        std::string                 _index;
        std::string                 _retrn;
        std::vector<int>            _allow_method;
        std::vector<std::string>    _cgi_path;
        std::vector<std::string>    _cgi_ext;
        std::string                 _block;
    /*methods*/

};

#endif