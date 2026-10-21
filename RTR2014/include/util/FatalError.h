#pragma once

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <utility>

/**
 * \brief Reports an error the application cannot continue from and ends it.
 *
 * The renderer cannot recover from a broken state, drawing the rest of the
 * scene would hide the error behind a wrong image. The error is logged and an
 * exception ends the application.
 */
template <typename... Args>
[[noreturn]] void fatalError(fmt::format_string<Args...> format, Args&&... args)
{
    const std::string message = fmt::format(format, std::forward<Args>(args)...);
    // The error is logged before the application ends, so that the log has the
    // reason even if the application does not shut down cleanly.
    spdlog::error("{}", message);
    spdlog::default_logger()->flush();
    throw std::runtime_error(message);
}

/**
 * \brief Reports an error the application cannot continue from and ends it.
 *
 * For error handlers that are called by C code, where an exception cannot be
 * thrown through the call stack safely.
 */
template <typename... Args>
[[noreturn]] void fatalExit(fmt::format_string<Args...> format, Args&&... args)
{
    const std::string message = fmt::format(format, std::forward<Args>(args)...);
    spdlog::error("{}", message);
    spdlog::default_logger()->flush();
    std::exit(1);
}
