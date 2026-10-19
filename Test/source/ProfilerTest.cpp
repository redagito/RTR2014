#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "util/Profiler.h"

namespace
{
std::string readFile(const std::string& path)
{
    std::ifstream in(path);
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}
} // namespace

TEST_CASE("Profiler records the count and total of a section", "[util]")
{
    Profiler profiler;
    {
        ScopedTimer timer(profiler, "section");
    }
    {
        ScopedTimer timer(profiler, "section");
    }
    {
        ScopedTimer timer(profiler, "other");
    }

    const std::string path = "rtr2014_tmp_profile.txt";
    profiler.writeReport(path);
    const std::string report = readFile(path);
    std::remove(path.c_str());

    REQUIRE(report.find("section") != std::string::npos);
    REQUIRE(report.find("other") != std::string::npos);
    // Both visits of "section" are counted together
    REQUIRE(report.find("2") != std::string::npos);
}

TEST_CASE("Profiler accepts timings from several threads", "[util]")
{
    Profiler profiler;
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i)
    {
        threads.emplace_back([&profiler]() {
            for (int j = 0; j < 100; ++j)
            {
                ScopedTimer timer(profiler, "parallel");
            }
        });
    }
    for (std::thread& thread : threads)
    {
        thread.join();
    }

    const std::string path = "rtr2014_tmp_profile_threads.txt";
    profiler.writeReport(path);
    const std::string report = readFile(path);
    std::remove(path.c_str());

    // 8 threads with 100 visits each
    REQUIRE(report.find("800") != std::string::npos);
}
