# ifndef PARSECONFIG_HPP
#define PARSECONFIG_HPP

#include <bitset>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <map>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <list>
#include <unistd.h>

typedef std::list < std::list < std::map < std::string, std::string > > > t_double_list;
typedef std::list < std::map < std::string, std::string > > t_single_list;

class ParseConfig
{
    private:

        // Constructor
            ParseConfig();
	        ParseConfig( std::string fileName );
            ParseConfig ( const ParseConfig &copy );
        
        // Assignement Operator
	        ParseConfig& operator=( ParseConfig &other );
  	    
        std::vector<serverConfig>   servers;
        std::vector<std::string>    server_str;
        int                         nbr_server;

    public:     
};

#endif