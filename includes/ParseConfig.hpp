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

enum tokenType
{
	BRACKOPEN = 0,
	BRACKCLOSE,
	SEMICOLON,
	NAME,
	VELUE,
	LOCATION,
	SERV,
	INIT
};

typedef std::list < std::list < std::map < std::string, std::string > > > t_double_list;
typedef std::list < std::map < std::string, std::string > > t_single_list;

//* this class  get config file and parse them and store it

class ParseConfig
{
    private:

        // Constructor
            ParseConfig();
	        ParseConfig( std::string fileName );
            ParseConfig ( const ParseConfig &copy );
        
        // Assignement Operator
	        void    operator=( ParseConfig &other );

        // Setters
            void    setDefaultConfig( void );
        
        // Getters
            int                                 getStartProcess( void );
            void                                getFile( void );
            t_double_list                       getList( void );
            t_double_list                       getListInterface( void );
            std::map<std::string, std:string>   getGlobalConfig( void );
            std::map<std::string, std:string>   getGlobalConfigInterface( void );
            std::vector<int>                    getPorts( void );
            std::vector<int>                    getPortsInterface( void );
            std::map<std::string, std:string>   getDefaultConfig( void );
            std::map<std::string, std:string>   getDefaultConfigInterface( void );

        // Methods
            void    handleComments( std::string &line );
            size_t  numberOfServerInterface( void );
            size_t  numberOfServer( void );
            bool    isNumber(std::string &str);
            void    checkPort(std::string &strPort);
            std::string createToken(size_t &i);

        // Members variables
            std::string                             _fileName;
            bool                                    _errorHappened;
            std::map<std::string, std::string>		_defautConfig;
            std::string                             _pwd;
            int                                     _bracketCounter;
            tokenType                               _previousToken;
            size_t                                  _indexConfigFile;
            bool                                    _location;
            t_double_list                           _serverList;
            std::map<std::string, std::string       _globalConfig;
            std::vector<int>                        _ports;
        //  unsigned long                           _clientMaxBodySize;
        // std::vector<serverConfig>   servers;
        // std::vector<std::string>    server_str;
        // int                         nbr_server;

    public:   

    static	ParseConfig &getInstance(std::string fileName);
	static	ParseConfig &getInstance();  
};

#endif