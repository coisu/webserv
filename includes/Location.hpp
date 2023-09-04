#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <ostream>
#include <sstream>
#include <map>
#include "utils.h"

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
        friend std::ostream& operator<<(std::ostream& os, const Location& location);
        //getters
        std::string getBlock() const;
        //setters
    private:
    /*methods*/
        void    setAttributes(std::string key, std::string value);
        void    initPath(std::string value);
        void    initRoot(std::string value);
        void    initIndex(std::string value);
        void    initAutoIndex(std::string value);
        void    initRet(std::string value);
        void    initAllowMethods(std::string value);
        void    initCGI(std::string value);

    /*attributes*/
        std::string                         _path;
        std::string                         _root;
        std::string                         _index;
        bool                                _autoIndex;
        std::string                         _ret;
        std::vector<int>                    _allowMethods;
        std::map<std::string, std::string>  _cgi;
        std::string                         _block;
};

#endif