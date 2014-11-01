#include "TimeStamp.h"

#include <ctime>
#include <string>
#include <cstdio>
#include <cstring>

std::string createTimeStamp()
{
	// Create timestamp
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[20];
	memset(&buffer[0], 0, 20);

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 20, "%Y%m%d%H%M%S", timeinfo);

	// Create log file name
	return std::string(buffer);
}