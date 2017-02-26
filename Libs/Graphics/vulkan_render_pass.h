#pragma once
#include <vulkan.hpp>
#include "swapchain_manager.h"

namespace Phyre {
namespace Graphics {

class SwapchainManager;
class VulkanRenderPass {
public:
    explicit VulkanRenderPass(const vk::Device& device, const SwapchainManager& swapchain_manager);
    ~VulkanRenderPass();

private:
    typedef std::vector<vk::ShaderModule> ShaderModuleVector;
    typedef std::vector<vk::Framebuffer> FramebufferVector;

    static vk::RenderPass InitializeRenderPass(const vk::Device& device, vk::SampleCountFlagBits num_samples, vk::Format image_format, vk::Format depth_format);
    static ShaderModuleVector InitializeShaderModules(const vk::Device& device);
    static FramebufferVector InitializeFramebuffers(const vk::Device& device,
                                                    const vk::ImageView& depth_image_view,
                                                    const vk::RenderPass& render_pass,
                                                    const uint32_t width,
                                                    const uint32_t height,
                                                    const SwapchainManager::SwapchainImageVector& swapchain_imge);

    // Reads the Spir-V bytecode into memory with proper data alignment
    static std::vector<uint32_t> ReadSpirV(const std::string file_name);

    // The logical device responsible for resource allocation and cleanup
    const vk::Device& device_;

    // The underlying render pass object
    vk::RenderPass render_pass_;

    // The shader module we will use in the render pass
    ShaderModuleVector shader_modules_;

    // The framebuffers we will use in the render pass
    FramebufferVector framebuffers_;

    // For logging
    static const std::string kWho;
};

}
}
