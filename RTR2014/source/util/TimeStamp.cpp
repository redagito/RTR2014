#include "util/TimeStamp.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>

std::string createTimeStamp()
{
    // Create timestamp
    time_t rawtime;
    struct tm timeinfo;
    char buffer[20];
    memset(buffer, 0, sizeof(buffer));

    time(&rawtime);
#ifdef _WIN32
    localtime_s(&timeinfo, &rawtime);
#else
    localtime_r(&rawtime, &timeinfo);
#endif
    strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", &timeinfo);

    // Create log file name
    return std::string(buffer);
}
