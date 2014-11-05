#pragma once

#include <list>
#include <unordered_map>
#include <string>

#include "ILogListener.h"

/**
 * \brief Stores debug data.
 * Stores log events in a ring buffer and key/value debug data pairs.
 */
class CDebugInfo : public ILogListener
{
   public:
    void notify(const std::string& level, const std::string& file, unsigned int line, const std::string& function,
                   const std::string& text);

    const std::list<std::string>& getLog() const;

    void setLogBufferSize(size_t size);
    size_t getLogBufferSize() const;

    void setValue(const std::string& key, const std::string& value);
    const std::unordered_map<std::string, std::string>& getValues() const;

   private:
    size_t m_logBufferSize = 10; /**< Maximum number of log events stored. */
    std::list<std::string> m_logBuffer; /**< Log buffer with latest log events. */
    std::unordered_map<std::string, std::string> m_values; /**< Stores debug key/value pairs. */
};
