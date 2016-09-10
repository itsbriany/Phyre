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
    static void trace(const std::string& message, const T& class_called_from)
    {
        Logger::trace(message, typeid(class_called_from).name());
    }

    template <typename T>
    static void debug(const std::string& message, const T& class_called_from)
    {
        Logger::debug(message, typeid(class_called_from).name());
    }

    template <typename T>
    static void info(const std::string& message, const T& class_called_from)
    {
        Logger::info(message, typeid(class_called_from).name());
    }

    template <typename T>
    static void warning(const std::string& message, const T& class_called_from)
    {
        Logger::warning(message, typeid(class_called_from).name());
    }

    template <typename T>
    static void error(const std::string& message, const T& class_called_from)
    {
        Logger::error(message, typeid(class_called_from).name());
    }

    template <typename T>
    static void fatal(const std::string& message, const T& class_called_from)
    {
        Logger::fatal(message, typeid(class_called_from).name());
    }

    
}
}
