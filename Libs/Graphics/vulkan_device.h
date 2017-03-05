#pragma once
#include <vulkan.hpp>
#include "vulkan_swapchain.h"
#include "vulkan_gpu.h"
#include "command_buffer_manager.h"
#include "vulkan_memory_manager.h"
#include "vulkan_pipeline.h"
#include "vulkan_render_pass.h"

namespace Phyre {
namespace Graphics {

/**
 * An abstraction around the core Vulkan Logical Device.
 * You may query various queues from here.
 */
class VulkanDevice {
public:
    explicit VulkanDevice(const VulkanGPU& gpu, const VulkanWindow& window);

    // Clean up allocated resources
    ~VulkanDevice();

    // Get a handle to the physical device
    const VulkanGPU& gpu() const { return gpu_; }

    // Get a handle to the logical device
    const vk::Device& get() const { return device_; }

    // Getters
    uint32_t graphics_queue_family_index() const { return graphics_queue_family_index_; }
    uint32_t presentation_queue_family_index() const { return presentation_queue_family_index_; }
    const vk::Queue& graphics_queue() const { return graphics_queue_; }
    const vk::Queue& presentation_queue() const { return presentation_queue_; }

    static std::vector<const char*> DeviceExtentionNames();

private:
    // Returns the index of the graphics queue that was found on the phyisical device
    static uint32_t InitializeGraphicsQueueIndex(const vk::PhysicalDevice& gpu);

    // Returns a vk::DeviceQueueCreateInfo associated with its physical device
    static vk::DeviceQueueCreateInfo PrepareGraphicsQueueInfo(const vk::PhysicalDevice& gpu, std::vector<float>& queue_priorities, uint32_t max_queue_count);

    // Throws a runtime exception if the logical device failed to instantiate
    static vk::Device InitializeLogicalDevice(const vk::PhysicalDevice& gpu);

    // Returns the index from the gpu's queue family with a queue which is capable of graphics and presentation
    static uint32_t InitializePresentationQueueIndex(const vk::PhysicalDevice& gpu, const vk::SurfaceKHR& surface, uint32_t graphics_queue_index);

    // Initializes the graphics queue
    static vk::Queue InitializeGraphicsQueue(vk::Device& device, uint32_t graphics_queue_family_index);

    // Initializes the presentation queue
    static vk::Queue InitializePresentationQueue(vk::Device& device,
                                                 const vk::Queue& graphics_queue,
                                                 uint32_t graphics_queue_family_index,
                                                 uint32_t presentation_queue_family_index);

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

    // The device queue used for graphics
    vk::Queue graphics_queue_;

    // The device queue used for presentation
    vk::Queue presentation_queue_;

    // Manages memory and allocates buffers
    //VulkanMemoryManager memory_manager_;

    // Manages the command pools and buffers
    //CommandBufferManager* p_command_buffer_manager_;

    // Points the the swapchain for image handling
    //VulkanSwapchainDeprecated* p_swapchain_;

    // A pointer to the Vulkan Pipeline
    //std::unique_ptr<VulkanPipeline> p_pipeline_;

    // For logging
    static const std::string kWho;
};

}
}
