#pragma once
#include "logger.h"

namespace GameEngine
{
namespace Logging
{


    static void set_log_level(LogLevel log_level = kDebug)
    {
        Logger::set_log_level(log_level);
    }

    template <typename T>
    static void trace(const std::string& message, const T& obj)
    {
        Logger::trace(message, obj);
    }

    template <typename T>
    static void debug(const std::string& message, const T& obj)
    {
        Logger::debug(message, obj);
    }

    template <typename T>
    static void info(const std::string& message, const T& obj)
    {
        Logger::info(message, obj);
    }

    template <typename T>
    static void warning(const std::string& message, const T& obj)
    {
        Logger::warning(message, obj);
    }

    template <typename T>
    static void error(const std::string& message, const T& obj)
    {
        Logger::error(message, obj);
    }

    template <typename T>
    static void fatal(const std::string& message, const T& obj)
    {
        Logger::fatal(message, obj);
    }

}
}
