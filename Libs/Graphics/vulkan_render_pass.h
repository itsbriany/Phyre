#pragma once

#include <vulkan.hpp>


namespace Phyre {
namespace Graphics {

class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass {
public:
    explicit VulkanRenderPass(const VulkanDevice& device, const VulkanSwapchain& swapchain);
    ~VulkanRenderPass();

    const vk::RenderPass& get() const { return render_pass_; }

private:
    // -------------------- Type Definitions -------------------


    // -------------------- Initialization -------------------
    static vk::RenderPass LoadRenderPass(const VulkanDevice& device, const VulkanSwapchain& swapchain);

    // -------------------- Data Members -------------------
    // The logical device responsible for resource allocation and cleanup
    const VulkanDevice& device_;

    // A reference to the swapchain manager
    const VulkanSwapchain& swapchain_;

    // The render pass to which we supply attachments
    vk::RenderPass render_pass_;

    // For logging
    static const std::string kWho;
};

}
}
