#pragma once

/**
* \brief Simple logging macros.
*/
#ifdef _DEBUG
#include "CLogger.h"
#include <cstring>

#define STRIPPED_FILE_NAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG_MESSAGE(fmt, ...)                                                              \
    CLogger::log(("Message [%s,%d %s]: " fmt), STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, \
                 ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)                                                              \
    CLogger::log(("Debug [%s,%d %s]: " fmt), STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, \
                 ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)                                                              \
    CLogger::log(("Info [%s,%d %s]: " fmt), STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, \
                 ##__VA_ARGS__)

#define LOG_WARNING(fmt, ...)                                                              \
    CLogger::log(("Warning [%s,%d %s]: " fmt), STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, \
                 ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)                                                              \
    CLogger::log(("Error [%s,%d %s]: " fmt), STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, \
                 ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...)                                                              \
    CLogger::log(("Critical [%s,%d %s]: " fmt), STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, \
                 ##__VA_ARGS__)
#define LOG_EXCEPTION(fmt, ...)                                                              \
    CLogger::log(("Exception [%s,%d %s]: " fmt), STRIPPED_FILE_NAME, __LINE__, __FUNCTION__, \
                 ##__VA_ARGS__)
#else
#define LOG_MESSAGE(fmt, ...)
#define LOG_DEBUG(fmt, ...)
#define LOG_INFO(fmt, ...)

#define LOG_WARNING(fmt, ...)
#define LOG_ERROR(fmt, ...)
#define LOG_CRITICAL(fmt, ...)
#define LOG_EXCEPTION(fmt, ...)
#endif
