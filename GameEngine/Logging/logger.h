#pragma once
#include "log_level.h"
#include <map>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/trivial.hpp>

namespace GameEngine
{
namespace Logging
{
    class Logger
    {
    public:

        // Any log call with the level above or equal to the specified lvl will be logged
        static void set_log_level(const LogLevel& lvl);

	template <typename T>
        static void trace(const std::string& message, const T& obj)
        {
            BOOST_LOG_TRIVIAL(trace) << obj << message;
        }

	template <typename T>
        static void debug(const std::string& message, const T& obj)
        {
            BOOST_LOG_TRIVIAL(debug) << obj << message;
        }

	template <typename T>
        static void info(const std::string& message, const T& obj)
        {
            BOOST_LOG_TRIVIAL(info) << obj << message;
        }

	template <typename T>
        static void warning(const std::string& message, const T& obj)
        {
            BOOST_LOG_TRIVIAL(warning) << obj << message;
        }

	template <typename T>
        static void error(const std::string& message, const T& obj)
        {
            BOOST_LOG_TRIVIAL(error) << obj << message;
        }

	template <typename T>
        static void fatal(const std::string& message, const T& obj)
        {
            BOOST_LOG_TRIVIAL(fatal) << obj << message;
        }

    public:
        static std::map<LogLevel, std::string> s_string_to_log_level_map;
        static std::map<std::string, LogLevel> s_level_to_string_map;
    };
}
}
