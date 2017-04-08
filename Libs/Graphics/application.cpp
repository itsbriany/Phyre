#include <Logging/logging.h>

#include "application.h"

namespace Phyre {
namespace Graphics {

const std::string Application::kWho = "[Graphics::Application]";

Application::Application() {
    PHYRE_LOG(trace, kWho) << "Instantiated";
}

Application::~Application() {
    PHYRE_LOG(trace, kWho) << "Destroyed";
}

}
}
