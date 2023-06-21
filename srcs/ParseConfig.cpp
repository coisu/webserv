#include "ParseConfig.hpp"

// Constructors
// this construct it called when there is no one given parameter to the program
ParseConfig::ParseConfig(): _fileName("./resources/default.conf"), _configFile(std::string()), _errorHappened(false){

    std::cout << "-----------\tRun with default file\t----------"
    setDefaultConfig();
    int res = getStartProcess();
    if (res == ERROR)
        _errorHappened(true);
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
void operator=( ParseConfig &other ){
    (void)other;
}

void	ParseConfig::setDefaultConfig( void ){
	
    _defautConfig["allow"] = "PUT GET DELETE POST";
	_defautConfig["listen"] = "8080";
	_defautConfig["client_max_size"] = "5000000";
	_defautConfig["error page"] = " 404 error.html";
	_defautConfig["server_name"] = "banana";
	_defautConfig["index"] = "index.html";
	_defautConfig["cgi_pass"] = "/usr/bin/php-cgi";
	_defautConfig["autoindex"] = "on";
	char *pwd = getcwd(NULL, 0);
	if (pwd)
	{
		_pwd = pwd;
		_defautConfig["root"] = pwd;
		_defautConfig["root"] += "/www";
		free(pwd);
	}
}

// Getters

int     ParseConfig::getStartProcess( void ){
    
    try{
        getFile();
		_bracketCounter = 0;
		_previousToken = INIT;
		_indexConfigFile = 0;
		_location = false;
		setDefaultConfig();
		_blockServer = _defautConfig;
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
			handleCommentes(_configFile);
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

std::map<std::string, std:string>	&ParseConfig::getGlobalConfig(){
	return(getInstance(severFileName).getGlobalConfigInterface());
}

std::map<std::string, std:string>   &ParseConfig::getGlobalConfigInterface(){
	return(_globalConfig);
}

std::vector<int>	&ParseConfig::getPortsInterface(){
	return(_ports);
}

std::vector<int>	&ParseConfig::getPorts(){
	return(getInstance(severFileName).getPortsInterface());
}

std::map<std::string, std:string>	&ParseConfig::getDefaultConfigInterface(){
	return(_defautConfig);
}

std::map<std::string, std:string>   &ParseConfig::getDefaultConfig(){
	return(getInstance(severFileName).getDefaultConfigInterface());
}

bool	Parse_config_file::	getErrorHappened(){return (_errorHappened);}

// Methods

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
	return(getInstance(severFileName).NumberOfServerInterface());
}

size_t	ParseConfig::numberOfServerInterface( void ){
	return(_serverList.size())
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

// /**
//  * @brief Get the Piece Of string it is a token it return a token on each call
//  * the goal is to get a word from string this word will be stored in @pieceOfString
//  * this word came from '_configFile'
//  * @_configFile is the config file
//  * @start is the begin of word 
//  * @nbCharacterTocopy number of character to copy from @start
//  * @param i is index of string _configFile 
//  * @return std::string 
//  */
// std::string	Parse_config_file::								create_token(size_t &i)
// {
// 	size_t	nbCharacterTocopy = 0;
// 	size_t start = i;

// 	if (_configFile[i] == '{' || _configFile[i] == '}' || _configFile[i] == ';')//if { or } or ; is the first character i going to catch that piece Of String
// 	{
// 		i++;
// 		nbCharacterTocopy++;
// 	}
// 	else
// 	{
// 		while (!isspace(_configFile[i]) && (_configFile[i] != '{' && _configFile[i] != '}') && _configFile[i] != ';') // { or } or ; and whitespace character are delimiter
// 		{
// 			nbCharacterTocopy++;
// 			i++;
// 		}
// 	}
// 	std::string pieceOfString = _configFile.substr(start, nbCharacterTocopy);
// 	return (pieceOfString);
// }
