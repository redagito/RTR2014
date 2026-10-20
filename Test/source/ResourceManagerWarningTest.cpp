#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>

#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "resource/ResourceManager.h"

namespace
{
class TempObj
{
   public:
    explicit TempObj(const std::string& content)
    {
        std::ofstream ofs(m_path);
        ofs << content;
    }
    ~TempObj() { std::remove(m_path.c_str()); }

    std::string path() const { return m_path; }

   private:
    std::string m_path = "rtr2014_tmp_warn.obj";
};

// Collects the messages the default logger writes.
class CapturingSink : public spdlog::sinks::base_sink<std::mutex>
{
   public:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.emplace_back(msg.payload.data(), msg.payload.size());
    }
    void flush_() override {}

    bool contains(const std::string& text) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const std::string& message : m_messages)
        {
            if (message.find(text) != std::string::npos)
            {
                return true;
            }
        }
        return false;
    }

   private:
    mutable std::mutex m_mutex;
    std::vector<std::string> m_messages;
};
}  // namespace

TEST_CASE("loadMesh reports a warning without failing the load", "[resource]")
{
    // References an mtl that is not present, which tinyobj reports as a warning.
    TempObj file(
        "mtllib does_not_exist.mtl\n"
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "f 1 2 3\n");

    const auto sink = std::make_shared<CapturingSink>();
    spdlog::default_logger()->sinks().push_back(sink);

    ResourceManager manager;
    const ResourceId id = manager.loadMesh(file.path());

    spdlog::default_logger()->sinks().pop_back();

    REQUIRE(id != invalidResource);
    REQUIRE(sink->contains("does_not_exist.mtl"));
}
