#include <catch2/catch_test_macros.hpp>

#include <memory>

#include <spdlog/spdlog.h>

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
} // namespace

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

TEST_CASE("DebugInfo stores key value pairs", "[debug]")
{
    auto debugInfo = std::make_shared<DebugInfo>();

    debugInfo->setValue("FPS", "60");

    REQUIRE(debugInfo->getValues().at("FPS") == "60");
}
