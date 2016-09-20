#include "logger.h"
#include <boost/assign/list_of.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace GameEngine
{
    namespace Logging
    {
        std::map<Logger::LogLevel, std::string> Logger::s_string_to_log_level_map = boost::assign::map_list_of
        (kTrace, "trace")
        (kDebug, "debug")
        (kInfo, "info")
        (kWarning, "warning")
        (kError, "error")
        (kFatal, "fatal");

        std::map<std::string, Logger::LogLevel> Logger::s_level_to_string_map = boost::assign::map_list_of
        ("trace", kTrace)
        ("debug", kDebug)
        ("info", kInfo)
        ("warning", kWarning)
        ("error", kError)
        ("fatal", kFatal);

        bool Logger::s_is_trace_enabled = true;
        bool Logger::s_is_debug_enabled = true;
        bool Logger::s_is_info_enabled = true;
        bool Logger::s_is_warning_enabled = true;
        bool Logger::s_is_error_enabled = true;
        bool Logger::s_is_fatal_enabled = true;

        void Logger::disable_lvl(LogLevel lvl)
        {
            switch(lvl)
            {
            case kTrace:
                s_is_trace_enabled = false;
                return;
            case kDebug:
                s_is_debug_enabled = false;
                return;
            case kInfo:
                s_is_info_enabled = false;
                return;
            case kWarning:
                s_is_warning_enabled = false;
                return;
            case kError:
                s_is_error_enabled = false;
                return;
            default:
                s_is_fatal_enabled = false;
            }
        }

        void Logger::disable_all()
        {
            s_is_trace_enabled = false;
            s_is_debug_enabled = false;
            s_is_info_enabled = false;
            s_is_warning_enabled = false;
            s_is_error_enabled = false;
            s_is_fatal_enabled = false;
        }

        void Logger::enable_lvl(LogLevel lvl)
        {
            switch (lvl)
            {
            case kTrace:
                s_is_trace_enabled = true;
                return;
            case kDebug:
                s_is_debug_enabled = true;
                return;
            case kInfo:
                s_is_info_enabled = true;
                return;
            case kWarning:
                s_is_warning_enabled = true;
                return;
            case kError:
                s_is_error_enabled = true;
                return;
            default:
                s_is_fatal_enabled = true;
            }
        }

        void Logger::enable_all()
        {
            set_log_level(kTrace);
            s_is_trace_enabled = true;
            s_is_debug_enabled = true;
            s_is_info_enabled = true;
            s_is_warning_enabled = true;
            s_is_error_enabled = true;
            s_is_fatal_enabled = true;
        }

        void Logger::set_log_level(const LogLevel& lvl)
        {
            // See more specific logging information
            boost::log::add_common_attributes();

            LogLevel s_log_level = lvl;

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
