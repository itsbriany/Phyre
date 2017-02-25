#include "vulkan_rendering_system.h"
#include "logging.h"

const std::string Phyre::Graphics::VulkanRenderingSystem::kWho = "[VulkanRenderingSystem]";

Phyre::Graphics::VulkanRenderingSystem::VulkanRenderingSystem() : loader_() { Logging::trace("Instantiated", kWho); }
Phyre::Graphics::VulkanRenderingSystem::~VulkanRenderingSystem() { Logging::trace("Destroyed", kWho); }

void Phyre::Graphics::VulkanRenderingSystem::Start() {
    // RAII?
}
