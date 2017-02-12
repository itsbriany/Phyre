#pragma once
#include <string>

namespace GameEngine {
namespace Logging {

    class LoggableInterface {
        public:
            virtual std::string log() = 0;
            virtual ~LoggableInterface() { }
    };

}
}
