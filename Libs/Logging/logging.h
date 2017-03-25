#pragma once
#include "loggable_interface.h"
#include "logger.h"

namespace Phyre {
namespace Logging {

#define PHYRE_LOG(boost_log_level, who) BOOST_LOG_TRIVIAL(boost_log_level) << who << ": "

inline void set_log_level(LogLevel log_level = kDebug) {
    Logger::set_log_level(log_level);
}

}
}
