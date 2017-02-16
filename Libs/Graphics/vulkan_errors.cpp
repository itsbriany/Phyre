#pragma once
#include "vulkan_errors.h"
#include "logging.h"

bool Phyre::Graphics::ErrorCheck(const std::string& message, const std::string& who) {
    Logging::error(message, who);
    return false;
}

bool Phyre::Graphics::ErrorCheck(vk::Result result, const std::string& who) {
    std::string message;

    switch (result) {
    case vk::Result::eSuccess:
    case vk::Result::eEventReset:
    case vk::Result::eEventSet:
    case vk::Result::eIncomplete:
    case vk::Result::eNotReady:
    case vk::Result::eSuboptimalKHR:
    case vk::Result::eTimeout:
        return true;
    case vk::Result::eErrorDeviceLost:
        message = "Device lost";
        return ErrorCheck(message, who);
    case vk::Result::eErrorExtensionNotPresent:
        message = "Extension not present";
        return ErrorCheck(message, who);
    case vk::Result::eErrorIncompatibleDriver:
        message = "Incompatible driver";
        return ErrorCheck(message, who);
    case vk::Result::eErrorFeatureNotPresent:
        message = "Feature not present";
        return ErrorCheck(message, who);
    case vk::Result::eErrorFormatNotSupported:
        message = "Format not supported";
        return ErrorCheck(message, who);
    case vk::Result::eErrorFragmentedPool:
        message = "Fragmented pool";
        return ErrorCheck(message, who);
    case vk::Result::eErrorIncompatibleDisplayKHR:
        message = "Incompatible display";
        return ErrorCheck(message, who);
    case vk::Result::eErrorInitializationFailed:
        message = "Initialization field error";
        return ErrorCheck(message, who);
    case vk::Result::eErrorInvalidShaderNV:
        message = "Invalid shader NV";
        return ErrorCheck(message, who);
    case vk::Result::eErrorLayerNotPresent:
        message = "Layer not present";
        return ErrorCheck(message, who);
    case vk::Result::eErrorMemoryMapFailed:
        message = "Memory map failed";
        return ErrorCheck(message, who);
    case vk::Result::eErrorNativeWindowInUseKHR:
        message = "Native window in use";
        return ErrorCheck(message, who);
    case vk::Result::eErrorOutOfDateKHR:
        message = "Out of date";
        return ErrorCheck(message, who);
    case vk::Result::eErrorOutOfDeviceMemory:
        message = "Device out of memory";
        return ErrorCheck(message, who);
    case vk::Result::eErrorOutOfHostMemory:
        message = "Host out of memory";
        return ErrorCheck(message, who);
    case vk::Result::eErrorOutOfPoolMemoryKHR:
        message = "Out of pool memory";
        return ErrorCheck(message, who);
    case vk::Result::eErrorSurfaceLostKHR:
        message = "Surface lost";
        return ErrorCheck(message, who);
    case vk::Result::eErrorTooManyObjects:
        message = "Too many objects";
        return ErrorCheck(message, who);
    case vk::Result::eErrorValidationFailedEXT:
        message = "Validation failed";
        return ErrorCheck(message, who);
    default:
        message = "Unknown error";
        return ErrorCheck(message, who);
    }
}