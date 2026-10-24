#pragma once

#include <spdlog/details/log_msg.h>
#include <spdlog/sinks/base_sink.h>

#include <list>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

/**
 * \brief Stores one row of debug data.
 */
struct DebugValue
{
    std::string key;
    std::string setting;
    std::string status;
};

/**
 * \brief Stores debug data.
 * Log sink that keeps events in a ring buffer, plus ordered debug rows.
 */
class DebugInfo : public spdlog::sinks::base_sink<std::mutex>
{
   public:
    std::list<std::string> getLog() const;

    void setLogBufferSize(size_t size);
    size_t getLogBufferSize() const;

    void setValues(std::vector<DebugValue> values);
    std::vector<DebugValue> getValues() const;

   private:
    void sink_it_(const spdlog::details::log_msg& msg) override;
    void flush_() override {}

    // Guards the members below against concurrent log-sink writes and
    // render-thread reads. Distinct from base_sink's mutex_, which is not
    // mutable and so cannot be locked from the const getters.
    mutable std::mutex m_mutex;

    size_t m_logBufferSize = 10;        /**< Maximum number of log events stored. */
    std::list<std::string> m_logBuffer; /**< Log buffer with latest log events. */
    std::vector<DebugValue> m_values;   /**< Debug rows in display order. */
};
