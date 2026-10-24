#include <spdlog/spdlog.h>

#include <catch2/catch_test_macros.hpp>
#include <memory>

#include "debug/DebugInfo.h"

namespace
{
// A logger of its own, so the tests do not change the sinks of the default
// logger that the rest of the program uses.
std::shared_ptr<spdlog::logger> makeLogger(const std::shared_ptr<DebugInfo>& debugInfo)
{
    auto logger = std::make_shared<spdlog::logger>("debug_info_test");
    logger->sinks().push_back(debugInfo);
    return logger;
}
}  // namespace

TEST_CASE("DebugInfo sink captures formatted log messages", "[debug]")
{
    auto debugInfo = std::make_shared<DebugInfo>();
    const auto logger = makeLogger(debugInfo);

    logger->error("disk {} is {}", "full", 42);

    const auto log = debugInfo->getLog();
    REQUIRE(log.size() == 1);
    REQUIRE(log.front() == "[error]: disk full is 42");
}

TEST_CASE("DebugInfo ring buffer keeps only the configured number of entries", "[debug]")
{
    auto debugInfo = std::make_shared<DebugInfo>();
    debugInfo->setLogBufferSize(2);
    const auto logger = makeLogger(debugInfo);

    logger->info("one");
    logger->info("two");
    logger->info("three");

    const auto log = debugInfo->getLog();
    REQUIRE(log.size() == 2);
    REQUIRE(log.back() == "[info]: three");
    REQUIRE(log.front() == "[info]: two");
}

TEST_CASE("DebugInfo stores debug rows in order", "[debug]")
{
    auto debugInfo = std::make_shared<DebugInfo>();

    debugInfo->setValues({
        {"F2", "Debug overlay", "On"},
        {"Mouse", "X", "120"},
        {"Mouse", "Y", "80"},
    });

    const auto values = debugInfo->getValues();
    REQUIRE(values.size() == 3);
    REQUIRE(values[0].key == "F2");
    REQUIRE(values[0].setting == "Debug overlay");
    REQUIRE(values[0].status == "On");
    REQUIRE(values[1].key == "Mouse");
    REQUIRE(values[1].setting == "X");
    REQUIRE(values[1].status == "120");
    REQUIRE(values[2].setting == "Y");
    REQUIRE(values[2].status == "80");
}
