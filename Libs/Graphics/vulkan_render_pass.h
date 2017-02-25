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
    typedef std::vector<vk::ShaderModule> ShaderModuleVector;

    static vk::RenderPass InitializeRenderPass(const vk::Device& device, vk::SampleCountFlagBits num_samples, vk::Format image_format, vk::Format depth_format);
    static ShaderModuleVector InitializeShaderModules(const vk::Device& device);

    // Reads the Spir-V bytecode into memory with proper data alignment
    static std::vector<uint32_t> ReadSpirV(const std::string file_name);

    // The logical device responsible for resource allocation and cleanup
    const vk::Device& device_;

    // The underlying render pass object
    vk::RenderPass render_pass_;

    // The shader module we will use in the render pass
    ShaderModuleVector shader_modules_;

    // For logging
    static const std::string kWho;
};

}
}
