#pragma once

#include <string>

/**
 * \brief Log listener interface.
 */
class ILogListener
{
   public:
	/**
	 * \biref Virtual destructor for interface class.
	 */
    virtual ~ILogListener();

    /**
     * \brief Notfies listener of log event.
     */
    virtual void notify(const std::string& logLevel, const std::string& file, unsigned int line, const std::string& function, const std::string& text) = 0;
};
