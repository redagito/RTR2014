#include "CLogger.h"

#include "Log.h"

#include <cstring>
#include <ctime>
#include <string>
#include <stdexcept>
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <sstream>

ILogListener::~ILogListener()
{

}

std::ofstream CLogger::s_stream;
std::set<ILogListener*> CLogger::s_listeners;

void CLogger::log(const char* level, const char* file, int line, const char* function,
                  const char* format, ...)
{
    // Text buffer
    char buffer[5000];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    std::stringstream ss;
    ss << level << " [" << file << ":" << line << ", " << function << "]: " << buffer;
    std::string detailedMessage = ss.str();

    // Print to standard output
    std::cout << detailedMessage << std::endl;
    // Print to file
    if (s_stream.is_open())
    {
        s_stream << detailedMessage << std::endl;
    }

    // passthrough to listeners
    for (ILogListener* l : s_listeners)
    {
        l->handleLog(level, file, line, function, std::string(buffer));
    }
}

bool CLogger::initLogFile(const std::string& logFile)
{
    if (!s_stream.is_open())
    {
        s_stream.open(logFile);
        if (!s_stream.is_open())
        {
            LOG_WARNING("Failed to open log file %s.", logFile.c_str());
            return false;
        }
        return true;
    }
    LOG_WARNING("Multiple logger initialization calls are invalid and will be ignored.");
    return false;
}

void CLogger::addListener(ILogListener* l) { s_listeners.insert(l); }

void CLogger::removeListener(ILogListener* l) { s_listeners.erase(l); }
