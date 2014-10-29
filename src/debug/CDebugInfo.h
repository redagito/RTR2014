#pragma once

#include <list>
#include <map>
#include <string>

#include "CLogger.h"

class CDebugInfo : public ILogListener
{
   public:
    void handleLog(const char* level, const char* file, int line, const char* function,
                   const std::string& text);

    const std::list<std::string>& getLog() const;

    void setLogBufferSize(size_t size);
    size_t getLogBufferSize() const;

    void setValue(const std::string& key, const std::string& value);
    const std::map<std::string, std::string>& getValues() const;

   private:
    size_t m_logBufferSize = 10;
    std::list<std::string> m_logBuffer;
    std::map<std::string, std::string> m_values;
};
