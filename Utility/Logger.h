#pragma once
/************************************************************************/
/* Logger
/* ------
/* Simple singleton logging class
/************************************************************************/

#include <fstream>
#include <sstream>
#include <string>


class Logger
{
public:
	~Logger();

	void log(const std::string& text);
	void log(const std::stringstream& text);
	Logger& operator<<(const std::string& text);
	Logger& operator<<(const std::stringstream& text);

	static Logger& get();

private:
	// Singleton - private ctors and assignment
	Logger();
	Logger(const Logger& that);
	Logger& operator=(const Logger& that);

	static const std::string logfilename;

	std::ofstream filestream;
};

inline void Log(const std::string& text)		{ Logger::get().log(text); }
inline void Log(const std::stringstream& text)  { Logger::get().log(text); }
