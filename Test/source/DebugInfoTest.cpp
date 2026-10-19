#include <catch2/catch_test_macros.hpp>

#include <spdlog/spdlog.h>

#include "debug/DebugInfo.h"

TEST_CASE("DebugInfo sink captures formatted log messages", "[debug]")
{
    spdlog::default_logger()->sinks().clear();

    auto debugInfo = std::make_shared<DebugInfo>();
    spdlog::default_logger()->sinks().push_back(debugInfo);

    spdlog::error("disk {} is {}", "full", 42);

    const auto log = debugInfo->getLog();
    REQUIRE(log.size() == 1);
    REQUIRE(log.front() == "[error]: disk full is 42");
}

TEST_CASE("DebugInfo ring buffer keeps only the configured number of entries", "[debug]")
{
    spdlog::default_logger()->sinks().clear();

    auto debugInfo = std::make_shared<DebugInfo>();
    debugInfo->setLogBufferSize(2);
    spdlog::default_logger()->sinks().push_back(debugInfo);

    spdlog::info("one");
    spdlog::info("two");
    spdlog::info("three");

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