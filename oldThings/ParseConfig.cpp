#include "ParseConfig.hpp"

// Constructors
// this construct it called when there is no one given parameter to the program
ParseConfig::ParseConfig(): _fileName("./resources/default.conf"), _configFile(std::string()), _errorHappened(false){

    std::cout << "-----------\tRun with default file\t----------"<< std::endl;
    setDefaultConfig();
    int res = getStartProcess();
    if (res == ERROR)
        _errorHappened = true;
}

ParseConfig & ParseConfig::	getInstance(std::string fileName)
{
	static  ParseConfig _instance(fileName);
	return (_instance);
}

static  std::string  severFileName;

 /* this construct it called when given parameter to the program
 	param fileName the config file */

ParseConfig::ParseConfig( std::string fileName ): _fileName( fileName ), _configFile( std::string() ), _errorHappened( false ){
	
	severFileName = fileName;
	setDefaultConfig();

	int res = getStartProcess();
	if (res == ERROR)
		_errorHappened =  true;
}

ParseConfig::ParseConfig ( const ParseConfig &copy ){
    (void)copy;
}
        
// Assignement Operator
void ParseConfig::operator=( ParseConfig &other ){
    (void)other;
}

// Destructor
ParseConfig::~ParseConfig(){}

// ----------------------------- Getters -------------------------------------

int     ParseConfig::getStartProcess( void ){
    
    try{
        getFile();
		_bracketCounter = 0;
		_previousToken = INIT;
		_indexConfigFile = 0;
		_isLocation = false;
		setDefaultConfig();
		_newServer = _defaultConfig;
		//parse();
		std::cout << ">> Config File [ OK ]" << std::endl;
	} catch(char const * errorMsg){
		std::cerr << errorMsg << std::endl;
		return (ERROR);
	}
	return (SUCCESS);
}

/* Get the File object
 * get file and store it in std::string _configFile
 * get file line by line and then concated each line got previously
 * contain the line of file */

void	ParseConfig:: getFile()
{
	std::string line;
	std::ifstream myReadFile(_fileName.c_str());
	if (myReadFile.is_open())
	{
		while (getline (myReadFile, line)) 
		{
			_configFile.append(line);
			handleComments(_configFile);
			_configFile.append("\n");
		}
		myReadFile.close();
	}
	else
		throw("error open file");
}

t_double_list &ParseConfig::getListInterface(){
	return(_serverList);
}

t_double_list &ParseConfig::getList(){
	return(getInstance(severFileName).getListInterface());
}

std::map<std::string, std::string>	&ParseConfig::getGlobalConfig(){
	return(getInstance(severFileName).getGlobalConfigInterface());
}

std::map<std::string, std::string>   &ParseConfig::getGlobalConfigInterface(){
	return(_globalConfig);
}

std::vector<int>	&ParseConfig::getPortsInterface(){
	return(_ports);
}

std::vector<int>	&ParseConfig::getPorts(){
	return(getInstance(severFileName).getPortsInterface());
}

std::map<std::string, std::string>	&ParseConfig::getDefaultConfigInterface(){
	return(_defaultConfig);
}

std::map<std::string, std::string>   &ParseConfig::getDefaultConfig(){
	return(getInstance(severFileName).getDefaultConfigInterface());
}

bool	ParseConfig::getErrorHappened(){ return (_errorHappened); }

tokenType	ParseConfig::getPrevToken(){ return(_previousToken); }

std::string	ParseConfig::getDirectiveName(){ return(_directiveName); }

std::string	ParseConfig::getDirectiveValue(){ return(_directiveValue); }

std::map<std::string, std::string>	ParseConfig::getNewServer(){ return(_newServer); }

std::map<std::string, std::string>	ParseConfig::getNewLocation(){ return(_newLocation); }

int	ParseConfig::getNewBracket(){ return(_bracketCounter); }

std::string	ParseConfig::getWord(){ return(_curWord); }

std::string	ParseConfig::getConfigFile(){ return(_configFile); }

size_t	ParseConfig::getIndexConfig(){ return(_indexConfigFile); }

t_single_list	ParseConfig::getSingleList(){ return(_sigleList); }

bool	ParseConfig::getIsLocation(){ return(_isLocation); }

std::map<std::string, std::string>	ParseConfig::getDefaultNewServer(){ return(_defaultConfig); }

std::string	ParseConfig::getDirectory(){ return(_pwd); }

//---------------------------- Setters -----------------------------------

void	ParseConfig::setDefaultConfig( void ){
	
    _defaultConfig["allow"] = "PUT GET DELETE POST";
	_defaultConfig["listen"] = "8080";
	_defaultConfig["client_max_size"] = "5000000";
	_defaultConfig["error page"] = " 404 error.html";
	_defaultConfig["server_name"] = "banana";
	_defaultConfig["index"] = "index.html";
	_defaultConfig["cgi_pass"] = "/usr/bin/php-cgi";
	_defaultConfig["autoindex"] = "on";
	char *pwd = getcwd(NULL, 0);
	if (pwd)
	{
		_pwd = pwd;
		_defaultConfig["root"] = pwd;
		_defaultConfig["root"] += "/www";
		free(pwd);
	}
}

void    ParseConfig::setPrevToken( tokenType tokenToSet ){ _previousToken = tokenToSet; }

void    ParseConfig::setDirectiveName( std::string name ){ _directiveName = name; }

void    ParseConfig::setDirectiveValue( std::string value ){ _directiveValue = value; }

void    ParseConfig::setNewServer( std::string name, std::string value ){ _newServer[name] = value; }

void    ParseConfig::setNewServer( std::map<std::string, std::string> newServer ){ _newServer = newServer; }

void    ParseConfig::setNewLocation( std::string name, std::string value ){ _newServer[name] = value; }

void    ParseConfig::setNewLocation( std::map<std::string, std::string> newLocation ){ _newLocation = newLocation; }

void    ParseConfig::setNewBracket( int value ){ _bracketCounter = value; }

void    ParseConfig::setWord( std::string word ){ _curWord = word; }

void    ParseConfig::setSigleList( t_single_list sList ){ _sigleList = sList; }

void    ParseConfig::setIsLocation( bool act ){ _isLocation = act; }


// --------------------------- Methods -----------------------------------

/* This function remove comments line in give string 
 * comment line start with "#"
 * param line which has # */

void	ParseConfig::handleComments(std::string &line)
{
	size_t  begin = line.find("#");
	if (begin != std::string::npos)
		line.erase(begin);
}

size_t	ParseConfig::numberOfServer( void ){
	return(getInstance(severFileName).numberOfServerInterface());
}

size_t	ParseConfig::numberOfServerInterface( void ){
	return(_serverList.size());
}

/* Returns true if given string in parameter is a number else return false
 * param str the given string to parse each character if is a digit character
 * return true  if given string in parameter is a number
 * return false if given string in parameter is not a number */

bool	ParseConfig::isNumber(std::string &str)
{
	int res;
	for (size_t i = 0; i < str.size(); i++)
	{
		res = isdigit(str[i]);
		if (res == 0)
			return (false);
	}
	return (true);
}

/* check if the port is a integer
 * if str_port is not a integer in string format a error will be  throw  
 * param str_port port in string format */
void	ParseConfig::checkPort(std::string &strPort)
{
	int intPort ;
	bool ret = isNumber(strPort);
	if (ret == false)
		throw("error : port must be a integer");
	std::stringstream(strPort) >> intPort; // convert string to integer
	_ports.push_back(intPort);
}

/* Get the Part Of string that is a token it return a token on each call
 * the goal is to get a word from string, an than this word will be stored in @partOfStr
 * this word came from '_configFile'
 * _configFile is the config file
 * start is the begin of word 
 * toCopy number of character to copy from start
 * param 'i' is index of string _configFile 
 * return partOfStr */
std::string	ParseConfig::createToken(size_t &i)
{
	size_t	toCopy = 0;
	size_t	start = i;

	if (_configFile[i] == '{' || _configFile[i] == '}' || _configFile[i] == ';')
	{
		i++;
		toCopy++;
	}
	else
	{
		while (!isspace(_configFile[i]) && (_configFile[i] != '{' && _configFile[i] != '}') && _configFile[i] != ';')
		{
			toCopy++;
			i++;
		}
	}
	std::string partOfStr = _configFile.substr(start, toCopy);
	return (partOfStr);
}

// static bool	isKeyWord(std::string &word)
// {
// 	if (word.compare("listen") == 0)
// 		return (true);
// 	if (word.compare("server_name") == 0)
// 		return (true);
// 	if (word.compare("root") == 0)
// 		return (true);
// 	if (word.compare("error_page") == 0)
// 		return (true);
// 	if (word.compare("autoindex") == 0)
// 		return (true);
// 	if (word.compare("client_body") == 0)
// 		return (true);
// 	if (word.compare("index") == 0)
// 		return (true);
// 	if (word.compare("methods") == 0)
// 		return (true);
// 	if (word.compare("allow") == 0)
// 		return (true);
// 	if (word.compare("cgi") == 0)
// 		return (true);
// 	if (word.compare("fastcgi_pass") == 0)
// 		return (true);
// 	if (word.compare("fastcgi_param") == 0)
// 		return (true);
// 	if (word.compare("return") == 0)
// 		return (true);
// 	if (word.compare("cgi_pass") == 0)
// 		return (true);
// 	return (false);
// }
