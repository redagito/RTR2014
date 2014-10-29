#pragma once

#include <fstream>
#include <set>
#include <string>

class ILogListener
{
   public:
    virtual void handleLog(const char* level, const char* file, int line, const char* function,
                           const std::string& text) = 0;
    virtual ~ILogListener();
};

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

    static void addListener(ILogListener*);
    static void removeListener(ILogListener*);

   private:
    static std::ofstream s_stream; /**< Log file stream. */

    // TODO make access to s_listeners thread safe?

    static std::set<ILogListener*> s_listeners;
};
