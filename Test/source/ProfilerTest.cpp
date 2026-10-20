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

// Returns the count the report lists for a section, or -1 if it is missing.
int countOf(const std::string& report, const std::string& section)
{
    std::istringstream stream(report);
    std::string line;
    while (std::getline(stream, line))
    {
        if (line.rfind(section, 0) != 0)
        {
            continue;
        }
        std::istringstream lineStream(line);
        std::string name;
        int count = -1;
        lineStream >> name >> count;
        return count;
    }
    return -1;
}
}  // namespace

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

    // Both visits of "section" are counted together
    REQUIRE(countOf(report, "section") == 2);
    REQUIRE(countOf(report, "other") == 1);
}

TEST_CASE("Profiler accepts timings from several threads", "[util]")
{
    Profiler profiler;
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i)
    {
        threads.emplace_back(
            [&profiler]()
            {
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
    REQUIRE(countOf(report, "parallel") == 800);
}
