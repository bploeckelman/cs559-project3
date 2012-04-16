/************************************************************************/
/* Logger
/* ------
/* Simple singleton logging class
/************************************************************************/
#include "Logger.h"

#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

using namespace std;


string getDateTimeString();


const string Logger::logfilename("log.txt");

Logger::Logger()
	: filestream(logfilename)
{
	if( !filestream.is_open() )
		throw std::exception("Error: unable to open log file for writing.");
	else
		log(std::string("Started logging: ") + getDateTimeString());
}

Logger::~Logger()
{
	log(std::string("\nStopped logging: ") + getDateTimeString());
	filestream.flush();
	filestream.close();
}

Logger& Logger::get()
{
	static Logger instance;
	return instance;
}

void Logger::log(const string& text)
{
	filestream << text << std::endl;
}

void Logger::log(const stringstream& text)
{
	filestream << text.str() << std::endl;
}

Logger& Logger::operator<<(const string& text)
{
	filestream << text;
	return *this;
}

Logger& Logger::operator<<(const stringstream& text)
{
	filestream << text.str();
	return *this;
}

#pragma warning(disable:4996)
string getDateTimeString()
{
	time_t t = time(0); // get time now
	struct tm* now = localtime(&t);

	stringstream ss;
	ss  << now->tm_mon <<"/"<< now->tm_mday <<"/"<< now->tm_year
		<< "  @  "
		<< now->tm_hour <<":"<< now->tm_min <<":"<< now->tm_sec << endl;
	return ss.str();
}
