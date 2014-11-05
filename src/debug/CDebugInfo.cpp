#include "CDebugInfo.h"

void CDebugInfo::notify(const std::string& level, const std::string& file, unsigned int line, const std::string& function,
                           const std::string& text)
{
    m_logBuffer.push_back("[" + level + "]: " + text);
    if (m_logBuffer.size() > m_logBufferSize)
    {
        m_logBuffer.pop_front();
    }
}

const std::list<std::string>& CDebugInfo::getLog() const { return m_logBuffer; }

void CDebugInfo::setLogBufferSize(size_t size)
{
    m_logBufferSize = size;
    while (m_logBufferSize > m_logBuffer.size())
    {
        m_logBuffer.pop_front();
    }
}

size_t CDebugInfo::getLogBufferSize() const { return m_logBufferSize; }

void CDebugInfo::setValue(const std::string& key, const std::string& value)
{
    m_values[key] = value;
}

const std::unordered_map<std::string, std::string>& CDebugInfo::getValues() const { return m_values; }
