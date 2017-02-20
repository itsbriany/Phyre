#pragma once
#include <vulkan.hpp>
#include "vulkan_swapchain.h"
#include "vulkan_gpu.h"

namespace Phyre {
namespace Graphics {

class VulkanDevice {
public:
    // Type definitions
    typedef std::vector<vk::CommandBuffer> CommandBufferVector;

    explicit VulkanDevice(const VulkanGPU& gpu, const VulkanWindow& window);

    // Clean up allocated resources
    ~VulkanDevice();

    // Get a handle to the physical device
    const VulkanGPU& GpuReference() const { return gpu_; }

    // Get a handle to the logical device
    const vk::Device& DeviceReference() const { return device_; }

    // Getters
    uint32_t graphics_queue_family_index() const { return graphics_queue_family_index_; }
    uint32_t presentation_queue_family_index() const { return presentation_queue_family_index_; }

    static std::vector<const char*> DeviceExtentionNames();

private:
    // Returns the index of the graphics queue that was found on the phyisical device
    static uint32_t InitializeGraphicsQueueIndex(const vk::PhysicalDevice& gpu);

    // Returns a vk::DeviceQueueCreateInfo associated with its physical device
    static vk::DeviceQueueCreateInfo PrepareGraphicsQueueInfo(const vk::PhysicalDevice& gpu, std::vector<float>& queue_priorities, uint32_t max_queue_count);

    // Throws a runtime exception if the logical device failed to instantiate
    static vk::Device InitializeLogicalDevice(const vk::PhysicalDevice& gpu);

    // Throws a runtime exception if the command pool failed to instantiate
    static vk::CommandPool InitializeCommandPool(const vk::Device& device, uint32_t graphics_queue_family_index);

    // Returns true if a command buffer could be created from the logical device
    static CommandBufferVector InitializeCommandBuffers(const vk::Device& device, const vk::CommandPool& command_pool);

    // Returns the index from the gpu's queue family with a queue which is capable of graphics and presentation
    static uint32_t InitializePresentationQueueIndex(const vk::PhysicalDevice& gpu, const vk::SurfaceKHR& surface, uint32_t graphics_queue_index);

    // A reference to the gpu we are using
    const VulkanGPU& gpu_;

    // The queue family index representing the queue family on the physical device
    // that has graphics capability
    uint32_t graphics_queue_family_index_;

    // The queue family index representing the queue family on the physical device
    // tha sends images to the surface
    uint32_t presentation_queue_family_index_;

    // The Vulkan logical device derived from one of the
    // VkInstance's physical devices
    vk::Device device_;

    // The command pool from which we create command buffers
    vk::CommandPool command_pool_;

    // The command buffer to which we send Vulkan commands to
    CommandBufferVector command_buffers_;

    // Points the the swapchain for image handling
    std::unique_ptr<VulkanSwapchain> p_swapchain_;

    // For logging
    static const std::string kWho;
};

}
}
