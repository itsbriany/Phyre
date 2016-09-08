#pragma once
#include "logger.h"

namespace GameEngine
{
namespace Logging
{

    static void config()
    {
        Logger::set_log_level(Logger::kDebug);
    }

    static void trace(const std::string& message)
    {
        Logger::trace(message);
    }

    static void debug(const std::string& message)
    {
        Logger::debug(message);
    }

    static void info(const std::string& message)
    {
        Logger::info(message);
    }

    static void warning(const std::string& message)
    {
        Logger::warning(message);
    }

    static void error(const std::string& message)
    {
        Logger::error(message);
    }

    static void fatal(const std::string& message)
    {
        Logger::fatal(message);
    }

    
}
}
