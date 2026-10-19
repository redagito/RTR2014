#pragma once

#include <list>
#include <mutex>
#include <string>
#include <unordered_map>

#include <spdlog/details/log_msg.h>
#include <spdlog/sinks/base_sink.h>

/**
 * \brief Stores debug data.
 * Log sink that keeps events in a ring buffer, plus key/value debug data pairs.
 */
class CDebugInfo : public spdlog::sinks::base_sink<std::mutex>
{
   public:
    std::list<std::string> getLog() const;

    void setLogBufferSize(size_t size);
    size_t getLogBufferSize() const;

    void setValue(const std::string& key, const std::string& value);
    std::unordered_map<std::string, std::string> getValues() const;

   private:
    void sink_it_(const spdlog::details::log_msg& msg) override;
    void flush_() override {}

    size_t m_logBufferSize = 10;        /**< Maximum number of log events stored. */
    std::list<std::string> m_logBuffer; /**< Log buffer with latest log events. */
    std::unordered_map<std::string, std::string> m_values; /**< Stores debug key/value pairs. */
};
