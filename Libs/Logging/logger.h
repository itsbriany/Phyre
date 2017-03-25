#pragma once
#include "log_level.h"
#include "loggable_interface.h"
#include <map>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#if defined (__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include <boost/log/trivial.hpp>
#pragma clang diagnostic pop
#else
#include <boost/log/trivial.hpp>
#endif

namespace Phyre {
namespace Logging {

class Logger {
public:
    // Any log call with the level above or equal to the specified lvl will be logged
    static void set_log_level(const LogLevel& lvl);
    static std::map<LogLevel, std::string> kStringToLogLevelMap;
    static std::map<std::string, LogLevel> kLevelToStringMap;
};

}
}
