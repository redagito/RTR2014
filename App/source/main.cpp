#include <exception>

#include <spdlog/spdlog.h>

#include "demo/RTRDemo.h"
#include "util/TimeStamp.h"
#include "util/Profiler.h"

namespace
{
void writeProfile()
{
    const std::string file = "log/profile_" + createTimeStamp() + ".txt";
    if (profiler().writeReport(file))
    {
        spdlog::info("Wrote profile to {}.", file);
    }
    else
    {
        spdlog::error("Failed to write the profile to {}.", file);
    }
}
} // namespace

int main()
{
    try
    {
        RTRDemo demo;

        int result = demo.init("data/demo.ini");
        if (result != 0)
        {
            spdlog::default_logger()->flush();
            return result;
        }

        result = demo.run();
        spdlog::info("Demo exited with code {}.", result);
        writeProfile();
        spdlog::default_logger()->flush();
        return result;
    }
    catch (const std::exception& e)
    {
        spdlog::error("Unhandled exception: {}", e.what());
        writeProfile();
        spdlog::default_logger()->flush();
        return 1;
    }
    catch (...)
    {
        spdlog::error("Unhandled exception of unknown type.");
        writeProfile();
        spdlog::default_logger()->flush();
        return 1;
    }
}