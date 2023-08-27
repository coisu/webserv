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
    std::cout << "\nLOCATION:\n" << line << "\n\n";
    // std::vector<std::string>    loc;
    // std::string                 word;
    // std::ifstream               ss(line.c_str());

    // while (std::getline(ss, word))
    //     loc.push_back(word);
    // if (loc.size() != 3)
    //     return false;
    // if (loc[0] != "location") return false;
    // if (loc[1][0] != '/') return false;
    // if (loc [2] != "{") return false;
    return (true);
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
    if (start >= str.size() || end >= str.size())
        throw std::runtime_error("cut out of range.");
    return (std::string(&str[start], &str[end]));
}

std::vector<Server> parseConfig( std::string configPath )
{
    std::ifstream       infile(configPath.c_str());
    // std::stringstream   buff;
    std::string         file;// = readFile(configPath);
    std::string         line;
    std::vector<Server> serverList;

    // buff << infile.rdbuf();
    // file = buff.str();
    // check if the config file opened correctly
    if (!infile.is_open())
        throw std::runtime_error("failed to open config file.");
    while (std::getline(infile, line))
    {
        if (removeSpaces(line)[0] == '#') continue ;
        file += removeSpaces(line);
    }
    size_t  start = 0;
    int     brackets = 0;
    for (size_t i = 0; i < file.size(); i++)
    {
        std::string             serverBlock;
        std::string             locationBlock;
        std::vector<Location>   locationList;
        
        if (brackets < 0 || brackets > 2)
            throw std::runtime_error("unmatched brackets");
        if (file[i] == '{')
        {
            brackets++;
            if (brackets == 1)
            {
                if ((line = cut(file, start, i)) != "server")
                    throw std::runtime_error("expected: \"server {\" got: \"" + line + "\".");
                start = i;
            }
            else if (brackets == 2)
            {
                size_t  lastSeperator = file.find_last_of(";}", i);
                if (!validLocation(line = cut(file, lastSeperator + 1, i)))// std::string(&file[lastSeperator + 1], &file[i])))//file.substr(lastSeperator, i)))
                    throw std::runtime_error("expected: \"location /foo/bar {\" got: \"" + line + "\"");
                serverBlock = cut(file, start, lastSeperator);
                std::cout << "server block: \"" << serverBlock << "\"\n";
            }
        }
        else if (file[i] == '}')
        {
            brackets--;
            if (brackets == 0)
                serverList.push_back(Server(serverBlock));
            else if (brackets == 1)
            {
                size_t openBrace = file.find_last_of("{", i);
                locationList.push_back(Location(file.substr(openBrace, i)));
            }
            else
                throw std::runtime_error("unmatched closing bracket");
        }
    }
    return (serverList);
}

// std::vector<Server> parseConfig( std::string configPath )
// {
//     std::ifstream       infile(configPath.c_str());
//     // char                ch;
//     int                 brackets = 0;
//     std::string         line;
//     std::vector<Server> serverList;
//     // while(infile.get(ch))
//     // {
//     //     if (ch == '{')
//     //         brackets++;
//     //     else if (ch == '}')
//     //     {
//     //         if (brackets == 0)
//     //             throw std::runtime_error("unmatched closing bracket.");
//     //         brackets--;
//     //     }
//     // }

//     // check if the config file opened correctly
//     if (!infile.is_open())
//         throw std::runtime_error("failed to open config file.");
//     // move line by line until something other than a comment or an empty line is found
//     while (std::getline(infile, line))
//     {
//         while (line == "\n" || trim(line)[0] == '#')
//             std::getline(infile, line);
//         if (trim(line) == "server {") // check that the line is the start of a server block
//         {
//             std::string             serverBlock;
//             std::vector<Location>   locationList;
//             brackets++;
//             while (std::getline(infile, line) && brackets > 0) // enter server block
//             {
//                 if (line == "\n" || trim(line)[0] == '#') continue ; //skip comments and empty lines
//                 if (line.find("}") != line.npos) // check for closing bracket and exit loop
//                 {
//                     brackets--;
//                     break ;
//                 }
//                 if (startsWith(trim(line), "location")) // check that the line is the start of a location block
//                 {
//                     std::string locationBlock(line);
//                     brackets++;
//                     if (validLocation(trim(line)))
//                         throw std::runtime_error("expected: \"location /foo/bar {\" got: \"" + line + "\"");
//                     while (std::getline(infile, line) && brackets > 1) // enter location block
//                     {
//                         if (line == "\n" || trim(line)[0] == '#') continue ; //skip comments and empty lines
//                         if (line.find("}") != line.npos) // check for closing bracket and exit loop
//                         {
//                             brackets--;
//                             break ;
//                         }
//                         locationBlock += trim(line);
//                     }
//                     locationList.push_back(Location(locationBlock));
//                 }
//                 serverBlock += trim(line);
//             }
//             serverList.push_back(Server(serverBlock));
//         }
//         else
//             throw std::runtime_error("expected: \"server {\" got: \"" + line + "\"");
//     }
//     return (serverList);
// }