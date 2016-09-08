#pragma once

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

        enum LogLevel
        {
            kTrace,
            kDebug,
            kInfo,
            kWarning,
            kError,
            kFatal
        };

        static void set_log_level(const LogLevel& lvl);

        static LogLevel log_level()
        {
            return s_log_level;
        }

        static void trace(const std::string& message)
        {
            BOOST_LOG_TRIVIAL(trace) << message;
        }

        static void debug(const std::string& message)
        {
            BOOST_LOG_TRIVIAL(debug) << message;
        }

        static void info(const std::string& message)
        {
            BOOST_LOG_TRIVIAL(info) << message;
        }

        static void warning(const std::string& message)
        {
            BOOST_LOG_TRIVIAL(warning) << message;
        }

        static void error(const std::string& message)
        {
            BOOST_LOG_TRIVIAL(error) << message;
        }

        static void fatal(const std::string& message)
        {
            BOOST_LOG_TRIVIAL(fatal) << message;
        }

    private:
        static LogLevel s_log_level;

    public:
        static std::map<LogLevel, std::string> s_string_to_log_level_map;
        static std::map<std::string, LogLevel> s_level_to_string_map;
    };
}
}
