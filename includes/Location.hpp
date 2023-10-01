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
		std::string getPath() const;
		bool		getIsCGI() const;
		std::map<std::string, std::string>		getCGIConfig() const;
        //setters
    private:
    /*methods*/
        void    setAttributes(std::string key, std::string value);
        void    initPath(std::string value);
        void    initAlias(std::string value);
        void    initIndex(std::string value);
        void    initAutoIndex(std::string value);
        void    initRet(std::string value);
        void    initAllowMethods(std::string value);
        void    initCGI(std::string value);

		void	initDefaults();

    /*attributes*/
        std::string                         _path;
        std::string                         _alias;
        std::string                         _index;
        bool                                _autoIndex;
        std::string                         _ret;
        std::vector<int>                    _allowMethods;
        std::map<std::string, std::string>  _cgiConfig;
        std::string                         _block;
		bool								_isCgi;
};

#endif