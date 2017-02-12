#pragma once
#include <string>

namespace Phyre {
namespace Logging {

    class LoggableInterface {
        public:
            virtual std::string log() = 0;
            virtual ~LoggableInterface() { }
    };

}
}
