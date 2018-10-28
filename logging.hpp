/*
	Declarations of functions for printing/logging all visual messages.
*/

#ifndef _LOGGING_HPP
#define _LOGGING_HPP

#include "globals.hpp"

#include <iostream>
#include <string>
#include <algorithm>

#define RED(x) std::string("\033[1;31m") + x + "\033[0m"
#define GREEN(x) std::string("\033[32m") + x + "\033[0m"
#define BLUE(x) std::string("\033[1;36m") + x + "\033[0m"
#define PURPLE(x) std::string("\033[1;35m") + x + "\033[0m"

void error_msg(std::string src, std::string msg);
void warning_msg(std::string src, std::string msg);
void log_msg(std::string src, std::string msg);
void status_msg(std::string src, std::string msg);

#endif