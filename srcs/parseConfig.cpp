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
    std::vector<std::string>    loc;
    std::string                 word;

    while (std::getline(ss, word))
        loc.push_back(word);
    if (loc.size() != 3)
        return false;
            
}

std::vector<Server> parseConfig( std::string configPath )
{
    std::ifstream       infile(configPath.c_str());
    char                ch;
    unsigned int        brackets = 0;
    std::string         line;
    std::vector<Server> servers;

    if (!infile.is_open())
        throw std::runtime_error("failed to open config file.");
    // while(infile.get(ch))
    // {
    //     if (ch == '{')
    //         brackets++;
    //     else if (ch == '}')
    //     {
    //         if (brackets == 0)
    //             throw std::runtime_error("unmatched closing bracket.");
    //         brackets--;
    //     }
    // }
    while (std::getline(infile, line))
    {
        if (line == "\n") continue ;
        else if (trim(line) == "server {")
        {
            brackets++;
            Server  server(serverBlock);
            Location    location(locationBlock);
            server.addLocation(location);
            while (std::getline(infile, line))
            {
                if (startsWith(trim(line), "location /"))
                {
                    if (!validLocation(trim(line)))
                        throw std::runtime_error("expected: \"location /* {\" got: \"" + line + "\"");
                }
            }
        }
        else
            throw std::runtime_error("expected: \"server {\" got: \"" + line + "\"");
    }
}