#include "debug/DebugInfo.h"

#include <spdlog/common.h>

void DebugInfo::sink_it_(const spdlog::details::log_msg& msg)
{
    const auto& levelName = spdlog::level::to_string_view(msg.level);

    std::string entry = "[";
    entry.append(levelName.data(), levelName.size());
    entry += "]: ";
    entry.append(msg.payload.data(), msg.payload.size());

    m_logBuffer.push_back(entry);
    if (m_logBuffer.size() > m_logBufferSize)
    {
        m_logBuffer.pop_front();
    }
}

std::list<std::string> DebugInfo::getLog() const { return m_logBuffer; }

void DebugInfo::setLogBufferSize(size_t size)
{
    m_logBufferSize = size;
    while (m_logBuffer.size() > m_logBufferSize)
    {
        m_logBuffer.pop_front();
    }
}

size_t DebugInfo::getLogBufferSize() const { return m_logBufferSize; }

void DebugInfo::setValue(const std::string& key, const std::string& value)
{
    m_values[key] = value;
}

std::unordered_map<std::string, std::string> DebugInfo::getValues() const
{
    return m_values;
}
