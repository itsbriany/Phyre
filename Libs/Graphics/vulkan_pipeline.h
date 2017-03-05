#pragma once

#include <vulkan.hpp>
#include "vulkan_device.h"
#include "vulkan_uniform_buffer.h"
#include "vulkan_swapchain.h"
#include "vulkan_render_pass.h"

namespace Phyre {
namespace Graphics {

// For more information on how the shader explicitly references descriptors in the shader language,
// check out https://vulkan.lunarg.com/doc/sdk/1.0.39.1/windows/tutorial/html/08-init_pipeline_layout.html
class VulkanPipeline {
public:
    typedef std::array<vk::WriteDescriptorSet, 2> WriteDescriptorSets;

    explicit VulkanPipeline(const VulkanDevice& device,
                            const VulkanSwapchain& swapchain);
    ~VulkanPipeline();


private:
    // A reference to the logical device
    const VulkanDevice& device_;

    // A reference to the swapchain manager
    const VulkanSwapchain& swapchain_;

    // Determines how many descriptor sets we will be using
    uint32_t descriptor_set_count_;

    // Holds a list of descriptor set layouts
    vk::PipelineLayout pipeline_layout_;

    // The render pass
    VulkanRenderPass render_pass_;

    // For logging
    static const std::string kWho;
};
}
}
