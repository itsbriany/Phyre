#pragma once
#include <vulkan.hpp>

namespace Phyre {
namespace Graphics {

class SwapchainManager;
class VulkanRenderPass {
public:
    explicit VulkanRenderPass(const vk::Device& device, const SwapchainManager& swapchain_manager);
    ~VulkanRenderPass();

private:
    static vk::RenderPass InitializeRenderPass(const vk::Device& device, vk::SampleCountFlagBits num_samples, vk::Format image_format, vk::Format depth_format);

    // The logical device responsible for resource allocation and cleanup
    const vk::Device& device_;

    // The underlying render pass object
    vk::RenderPass render_pass_;

    // For logging
    static const std::string kWho;
};

}
}
