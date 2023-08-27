#include "parseConfig.hpp"

// helper function to see if a string ('str') starts with a given string ('match')
bool    startsWith( std::string str, std::string match )
{
    if (str.length() < match.length())
        return false;
    return (str.substr(0, match.length()) == match);
}

// helper function to trim leading and trailing whitespace from a string
std::string trim( const std::string &str )
{
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    if (start == std::string::npos || end == std::string::npos)
        return ("");
    else
        return (str.substr(start, end - start + 1));
}

// helper function to verify location line e.g: "location /foo/bar {"
bool    validLocation( std::string line )
{
    // std::cout << "\nLOCATION:\n" << line << "\n\n";
    std::string                 word("location/");

    return (line.substr(0, word.size()) == word);
}

std::string removeSpaces(const std::string& input) {
    std::string result = "";
    for (std::string::size_type i = 0; i < input.length(); ++i) {
        if (!isspace(input[i])) {
            result += input[i];
        }
    }
    return result;
}

std::string cut(std::string str, size_t start, size_t end)
{
    // if (start >= str.size() || end >= str.size())
        // throw std::runtime_error("cut out of range.");
    start++;
    if (start >= str.size())
        throw std::runtime_error("cut out of range.");
        // start = str.size() - 1;
    if (end >= str.size())
        end = str.size() - 1;
    // std::cout << "start: " << start << "end " << end << std::endl;
    return (std::string(&str[start], &str[end]));
}

std::vector<Server> parseConfig( std::string configPath )
{
    std::ifstream       infile(configPath.c_str());
    std::string         file;// = readFile(configPath);
    std::string         line;
    std::vector<Server> serverVec;

    if (!infile.is_open())
        throw std::runtime_error("failed to open config file.");
    while (std::getline(infile, line))
    {
        if (removeSpaces(line)[0] == '#') continue ;
        file += removeSpaces(line);
    }
    size_t  start = -1;
    int     brackets = 0;
    bool    inLocation = false;
    // bool    inServer = false;
    std::string             serverBlock;
    std::vector<Location>   locationVec;
    for (size_t i = 0; i < file.size(); i++)
    {
        if (brackets < 0 || brackets > 2)
            throw std::runtime_error("unmatched brackets");
        if (file[i] == '{')
        {
            brackets++;
            if (brackets == 1)
            {
                if ((line = cut(file, start, i)) != "server")
                    throw std::runtime_error("expected: \"server {\" got: \"" + line + "\".");
            }
            else if (brackets == 2)
            {
                size_t  lastSeperator = file.find_last_of(";}", i);
                if (!validLocation(line = cut(file, lastSeperator, i)))// std::string(&file[lastSeperator + 1], &file[i])))//file.substr(lastSeperator, i)))
                    throw std::runtime_error("expected: \"location /foo/bar {\" got: \"" + line + "\"");
                if (!inLocation)
                    serverBlock += cut(file, start, lastSeperator+1);
                inLocation = true;
            }
            start = i;
        }
        else if (file[i] == '}')
        {
            brackets--;
            if (brackets == 0)
            {
                serverVec.push_back(Server(serverBlock, locationVec));
                inLocation = false;
                serverBlock.clear();
                locationVec.clear();
            }
            else if (brackets == 1)
            {
                size_t openBracket = file.find_last_of("{", i);

                locationVec.push_back(Location(line + ";" + cut(file, openBracket, i)));
                inLocation = false;
            }
            else
                throw std::runtime_error("unmatched closing bracket");
            start = i;
        }
    }
    if (brackets != 0)
        throw std::runtime_error("unmatched brackets");
    return (serverVec);
}
