#pragma once
#include "loggable_interface.h"
#include "logger.h"

namespace Phyre
{
namespace Logging
{

	inline void set_log_level(LogLevel log_level = kDebug)
    {
        Logger::set_log_level(log_level);
    }

	inline void trace(const std::string& message, LoggableInterface& loggable)
    {
        Logger::trace(message, loggable);
    }

	inline void debug(const std::string& message, LoggableInterface& loggable)
    {
        Logger::debug(message, loggable);
    }

	inline void info(const std::string& message, LoggableInterface& loggable)
    {
        Logger::info(message, loggable);
    }

	inline void warning(const std::string& message, LoggableInterface& loggable)
    {
        Logger::warning(message, loggable);
    }

	inline void error(const std::string& message, LoggableInterface& loggable)
    {
        Logger::error(message, loggable);
    }

	inline void fatal(const std::string& message, LoggableInterface& loggable)
    {
        Logger::fatal(message, loggable);
    }

}
}
