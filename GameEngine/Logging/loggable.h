#pragma once
#include <string>

namespace GameEngine {
namespace Logging {

    class Loggable {
        public:
            virtual std::string log() = 0;
            virtual ~Loggable() { }
    };

}
}
