#pragma once
#include "log_level.h"
#include "loggable_interface.h"
#include <map>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/trivial.hpp>

namespace Phyre {
namespace Logging {
class Logger {
public:
    // Any log call with the level above or equal to the specified lvl will be logged
    static void set_log_level(const LogLevel& lvl);

    static void trace(const std::string& message, LoggableInterface& loggable) {
        BOOST_LOG_TRIVIAL(trace) << loggable.log() << ' ' << message;
    }

    static void trace(const std::string& message, const std::string& who) {
        BOOST_LOG_TRIVIAL(trace) << who << ' ' << message;
    }

    static void debug(const std::string& message, LoggableInterface& loggable) {
        BOOST_LOG_TRIVIAL(debug) << loggable.log() << ' ' << message;
    }

    static void debug(const std::string& message, const std::string& who) {
        BOOST_LOG_TRIVIAL(debug) << who << ' ' << message;
    }

    static void info(const std::string& message, LoggableInterface& loggable) {
        BOOST_LOG_TRIVIAL(info) << loggable.log() << ' ' << message;
    }

    static void info(const std::string& message, const std::string& who) {
        BOOST_LOG_TRIVIAL(info) << who << ' ' << message;
    }

    static void warning(const std::string& message, LoggableInterface& loggable) {
        BOOST_LOG_TRIVIAL(warning) << loggable.log() << ' ' << message;
    }

    static void warning(const std::string& message, const std::string& who) {
        BOOST_LOG_TRIVIAL(warning) << who << ' ' << message;
    }

    static void error(const std::string& message, LoggableInterface& loggable) {
        BOOST_LOG_TRIVIAL(error) << loggable.log() << ' ' << message;
    }

    static void error(const std::string& message, const std::string& who) {
        BOOST_LOG_TRIVIAL(error) << who << ' ' << message;
    }

    static void fatal(const std::string& message, LoggableInterface& loggable) {
        BOOST_LOG_TRIVIAL(fatal) << loggable.log() << ' ' << message;
        throw std::runtime_error(message);
    }

    static void fatal(const std::string& message, const std::string& who) {
        BOOST_LOG_TRIVIAL(fatal) << who << ' ' << message;
        throw std::runtime_error(message);
    }

    static std::map<LogLevel, std::string> s_string_to_log_level_map;
    static std::map<std::string, LogLevel> s_level_to_string_map;
};

}
}
