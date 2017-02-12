#include "logger.h"
#include <boost/assign/list_of.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace Phyre {
namespace Logging {

std::map<LogLevel, std::string> Logger::s_string_to_log_level_map = boost::assign::map_list_of
(kTrace, "trace")
(kDebug, "debug")
(kInfo, "info")
(kWarning, "warning")
(kError, "error")
(kFatal, "fatal");

std::map<std::string, LogLevel> Logger::s_level_to_string_map = boost::assign::map_list_of
("trace", kTrace)
("debug", kDebug)
("info", kInfo)
("warning", kWarning)
("error", kError)
("fatal", kFatal);

void Logger::set_log_level(const LogLevel& lvl) {
    // See more specific logging information
    boost::log::add_common_attributes();

    switch (lvl)
    {
    case kTrace:
        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::trace
        );
        break;

    case kInfo:
        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::info
        );
        break;

    case kWarning:
        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::warning
        );
        break;

    case kError:
        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::error
        );
        break;

    case kFatal:
        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::fatal
        );
        break;

    default:
        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::debug
        );
        break;
    }
}
}
}
