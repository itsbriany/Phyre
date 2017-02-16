#pragma once
#include "vulkan_debugger.h"
#include "logging.h"

namespace Phyre {
namespace Graphics {

// Return true if the error check passed
bool ErrorCheck(const std::string& message, const std::string& who);
bool ErrorCheck(vk::Result result, const std::string& who);

}
}
