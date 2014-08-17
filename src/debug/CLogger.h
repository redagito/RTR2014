#pragma once

#include <fstream>
#include <string>

/**
* \brief Logger, prints log statements to console and, if set, to a valid log file.
*/
class CLogger
{
   public:
    /**
    * \brief Logs text.
    */
    static void log(const char* format, ...);

    /**
    * \brief Sets logging output to specified log file.
    */
    static bool initLogFile(const std::string& logFile);

   private:
    static std::ofstream s_stream; /**< Log file stream. */
};
