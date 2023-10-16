#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP
#include <vector>
#include <string>
#include <iostream>
#include "Server.hpp"
// #include <sstream>
#include "utils.hpp"
#include <fstream>
#include <cstdio>

std::vector<Server> parseConfig(std::string configPath);
#endif