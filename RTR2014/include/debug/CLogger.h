#pragma once

#include <fstream>
#include <list>
#include <string>

class ILogListener;

/**
* \brief Logger, prints log statements to console and, if set, to a valid log file.
*/
class CLogger
{
   public:
    /**
    * \brief Logs text.
    */
    static void log(const char* level, const char* file, int line, const char* function,
                    const char* format, ...);

    /**
    * \brief Sets logging output to specified log file.
    */
    static bool initLogFile(const std::string& logFile);

    static void addListener(ILogListener* listener);
    static void removeListener(ILogListener* listener);

   private:
    static std::ofstream s_stream; /**< Log file stream. */

    // TODO make access to s_listeners thread safe?
    static std::list<ILogListener*> s_listeners;
};
