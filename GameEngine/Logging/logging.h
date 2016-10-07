#pragma once
#include "logger.h"

namespace GameEngine
{
namespace Logging
{

    static void disable_all()
    {
        Logger::disable_all();
    }

    static void disable_lvl(Logger::LogLevel log_level)
    {
        Logger::disable_lvl(log_level);
    }

    static void enable_all()
    {
        Logger::enable_all();
    }

    static void enable_lvl(Logger::LogLevel log_level)
    {
        Logger::enable_lvl(log_level);
    }

    static void set_log_level(Logger::LogLevel log_level = Logger::kDebug)
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
