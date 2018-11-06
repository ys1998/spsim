/*
	Definitions of functions for printing/logging all visual messages.
*/

#include "logging.hpp"

// Function to print an error message and exit
void error_msg(std::string src, std::string msg){
	std::cout << RED("ERROR\t[" + src + "]\t" + msg) << std::endl;
	exit(1);
}

// Function to pring a status message
void status_msg(std::string src, std::string msg){
	std::cout << GREEN("STATUS\t[" + src + "]\t" + msg) << std::endl;
}

// Function to print logging message
void log_msg(std::string src, std::string msg){
	std::cout << std::string("LOG\t[" + src + "]\t") + msg << std::endl;
}

// Function to print warnings
void warning_msg(std::string src, std::string msg){
	std::cout << PURPLE("WARNING\t[" + src + "]\t" + msg) << std::endl;
}
