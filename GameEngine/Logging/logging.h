#pragma once
#include "loggable.h"
#include "logger.h"

namespace GameEngine
{
namespace Logging
{

    static void set_log_level(LogLevel log_level = kDebug)
    {
        Logger::set_log_level(log_level);
    }

    static void trace(const std::string& message, Loggable& loggable)
    {
        Logger::trace(message, loggable);
    }

    static void debug(const std::string& message, Loggable& loggable)
    {
        Logger::debug(message, loggable);
    }

    static void info(const std::string& message, Loggable& loggable)
    {
        Logger::info(message, loggable);
    }

    static void warning(const std::string& message, Loggable& loggable)
    {
        Logger::warning(message, loggable);
    }

    static void error(const std::string& message, Loggable& loggable)
    {
        Logger::error(message, loggable);
    }

    static void fatal(const std::string& message, Loggable& loggable)
    {
        Logger::fatal(message, loggable);
    }

}
}
