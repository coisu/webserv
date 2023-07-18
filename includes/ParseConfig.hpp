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
#include "utils.h"

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
	public:

		// Constructor
			ParseConfig();
			ParseConfig( std::string fileName );
			ParseConfig ( const ParseConfig &copy );
		// Assignement Operator
			void	operator=( ParseConfig &other );
		// Destructor
			~ParseConfig();
		
		// Getters
			int									getStartProcess( void );
			void								getFile( void );
			t_double_list						&getListInterface( void );
			std::map<std::string, std::string>	&getGlobalConfigInterface( void );
			std::vector<int>					&getPortsInterface( void );
			std::map<std::string, std::string>	&getDefaultConfigInterface( void );
			tokenType							getPrevToken( void );
			std::string							getDirectiveName( void );
			std::string							getDirectiveValue( void );
			std::map<std::string, std::string>	getNewServer( void );
			std::map<std::string, std::string>	getNewLocation( void );
			int									getNewBracket( void );
			std::string							getWord( void );
			std::string							getConfigFile( void );
			size_t								getIndexConfig( void );
			t_single_list						getSingleList( void );
			bool								getIsLocation( void );
			std::map<std::string, std::string>	getDefaultNewServer( void );
			std::string							getDirectory( void );

		// Setters
			void	setDefaultConfig( 	);
			void	setPrevToken( tokenType tokenToSet );
			void	setDirectiveName( std::string name );
			void	setDirectiveValue( std::string value );
			void	setNewServer( std::string name, std::string value );
			void	setNewServer( std::map<std::string, std::string> newServer );
			void	setNewLocation( std::string name, std::string value );
			void	setNewLocation( std::map<std::string, std::string> newLocation );
			void	setNewBracket( int value );
			void	setWord( std::string word );
			void	setSigleList( t_single_list sList );
			void	setIsLocation( bool act );

		// Methods
			void		handleComments( std::string &line );
			size_t		numberOfServerInterface( void );
			bool		isNumber(std::string &str);
			void		checkPort(std::string &strPort);
			std::string	createToken(size_t &i);

	private:
		// Members variables
		std::string                             _fileName;
		std::string                             _configFile;
		bool                                    _errorHappened;
		std::map<std::string, std::string>		_defaultConfig;
		std::string                             _pwd;
		int                                     _bracketCounter;
		tokenType                               _previousToken;
		size_t                                  _indexConfigFile;
		bool                                    _isLocation;
		std::map<std::string, std::string>		_newServer;
		std::map<std::string, std::string>		_newLocation;
		t_double_list                           _serverList;
		std::map<std::string, std::string>      _globalConfig;
		std::vector<int>                        _ports;
		std::string                             _directiveName;
		std::string                             _directiveValue;
		std::string                             _curWord;
		t_single_list                           _sigleList;

		//  unsigned long                           _clientMaxBodySize;
		// std::vector<serverConfig>   servers;
		// std::vector<std::string>    server_str;
		// int                         nbr_server;

		static  size_t                              numberOfServer( void );
		bool                                        getErrorHappened( void );
		static  std::map<std::string, std::string>  &getDefaultConfig( void );
		static  std::map<std::string, std::string>  &getGlobalConfig( void );
		static  ParseConfig                         &getInstance(std::string fileName);
		static  ParseConfig                         &getInstance();
		static  t_double_list                       &getList( void );
		static  std::vector<int>                    &getPorts( void ); 
};

#endif