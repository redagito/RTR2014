#include <exception>

#include <spdlog/spdlog.h>

#include "demo/RTRDemo.h"

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
        spdlog::default_logger()->flush();
        return result;
    }
    catch (const std::exception& e)
    {
        spdlog::error("Unhandled exception: {}", e.what());
        spdlog::default_logger()->flush();
        return 1;
    }
    catch (...)
    {
        spdlog::error("Unhandled exception of unknown type.");
        spdlog::default_logger()->flush();
        return 1;
    }
}