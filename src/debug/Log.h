#pragma once

/**
* \brief Simple logging macros.
*/
#if defined(_DEBUG) || defined(DEBUG)
#include "CLogger.h"
#include <cstring>

#define STRIPPED_FILE_NAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG_MESSAGE(fmt, ...) \
    CLogger::log("Message", STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) \
    CLogger::log("Debug", STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) \
    CLogger::log("Info", STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_WARNING(fmt, ...) \
    CLogger::log("Warning", STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) \
    CLogger::log("Error", STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) \
    CLogger::log("Critical", STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_EXCEPTION(fmt, ...) \
    CLogger::log("Exception", STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#else

#define LOG_MESSAGE(fmt, ...)
#define LOG_DEBUG(fmt, ...)
#define LOG_INFO(fmt, ...)

#define LOG_WARNING(fmt, ...)
#define LOG_ERROR(fmt, ...)
#define LOG_CRITICAL(fmt, ...)
#define LOG_EXCEPTION(fmt, ...)

#endif
