#include "parseConfig.hpp"

// helper function to see if a string ('str') starts with a given string ('match')
bool    startsWith( std::string str, std::string match )
{
    if (str.length() < match.length())
        return false;
    return (str.substr(0, match.length()) == match);
}

// helper function to verify location line e.g: "location /foo/bar {"
bool    validLocation( std::string line )
{
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
    start++;
    if (start >= str.size())
        throw std::runtime_error("cut out of range.");
    if (end >= str.size())
        end = str.size() - 1;
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
            if (brackets == 1) //server block start
            {
                if ((line = cut(file, start, i)) != "server")
                    throw std::runtime_error("expected: \"server {\" got: \"" + line + "\".");
            }
            else if (brackets == 2) //location
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
            if (!inLocation )
            {
                size_t  lastSeperator = file.find_last_of(";}", i);
                serverBlock += cut(file, start, lastSeperator );
            }
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
