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

// Methods

/* This function remove comments line in give string 
 * comment line start with "#"
 * param line which has # */

void	ParseConfig::handleCommentes(std::string &line)
{
	size_t  begin = line.find("#");
	if (begin != std::string::npos)
		line.erase(begin);
}

size_t	ParseConfig::numberOfServer( void ){
	return(getInstance(severFileName).interfaceNumberOfServer());
}

size_t	ParseConfig::interfaceNumberOfServer( void ){
	return(_serverList.size())
}

// Parse_config_file & Parse_config_file::							getInstance(std::string fileName)
// {
// 	// s_fileName = fileName;
// 	static  Parse_config_file  _instance(fileName);
// 	return (_instance);
// }


bool	Parse_config_file::	getErrorHappened(){return (_errorHappened);}