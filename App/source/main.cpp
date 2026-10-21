#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>
#include <string>

#include "demo/RTRDemo.h"
#include "util/Profiler.h"
#include "util/TimeStamp.h"

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
}  // namespace

int main(int argc, char** argv)
{
    try
    {
        RTRDemo demo;

        // The capture renders a number of frames and writes the last one to a
        // file, it does not show the window.
        std::string captureFile;
        int captureFrames = 1;
        for (int i = 1; i < argc; ++i)
        {
            const std::string argument = argv[i];
            if (argument == "--capture" && i + 1 < argc)
            {
                captureFile = argv[++i];
            }
            else if (argument == "--frames" && i + 1 < argc)
            {
                captureFrames = std::atoi(argv[++i]);
            }
            else
            {
                spdlog::error("Unknown argument {}.", argument);
                return 2;
            }
        }
        if (!captureFile.empty())
        {
            demo.setFrameCapture(captureFile, captureFrames);
        }

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