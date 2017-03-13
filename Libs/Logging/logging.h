#pragma once
#include "loggable_interface.h"
#include "logger.h"

namespace Phyre {
namespace Logging {

#define PHYRE_LOG(trace, who) BOOST_LOG_TRIVIAL(trace) << who << ": "
#define PHYRE_LOG(debug, who) BOOST_LOG_TRIVIAL(debug) << who << ": "
#define PHYRE_LOG(info, who) BOOST_LOG_TRIVIAL(info) << who << ": "
#define PHYRE_LOG(warning, who) BOOST_LOG_TRIVIAL(warning) << who << ": "
#define PHYRE_LOG(error, who) BOOST_LOG_TRIVIAL(error) << who << ": "
#define PHYRE_LOG(fatal, who) BOOST_LOG_TRIVIAL(fatal) << who << ": "

inline void set_log_level(LogLevel log_level = kDebug) {
    Logger::set_log_level(log_level);
}

}
}
