#pragma once
#include <vulkan.hpp>
#include "vulkan_swapchain_deprecated.h"
#include "vulkan_memory_manager.h"

namespace Phyre {
namespace Graphics {
class VulkanPipeline;

class VulkanSwapchainDeprecated;
class VulkanRenderPass {
public:
    typedef std::array<vk::VertexInputAttributeDescription, 2> VertexInputAttributeDescriptions;
    typedef std::vector<vk::PipelineShaderStageCreateInfo> PipelineShaderStages;

    struct VertexBuffer {
        vk::Buffer buffer;
        vk::DeviceMemory device_memory;
    };

    explicit VulkanRenderPass(const vk::Device& device, const VulkanSwapchainDeprecated& swapchain_manager, const VulkanMemoryManager& memory_manager);
    ~VulkanRenderPass();

    const vk::VertexInputBindingDescription& vertex_input_binding_description() const { return vertex_input_binding_description_; }
    const VertexInputAttributeDescriptions& vertex_input_attribute_descriptions() const { return vertex_input_attribute_descriptions_; }
    const PipelineShaderStages& pipeline_shader_stages() const { return pipeline_shader_stages_; }
    const vk::RenderPass& render_pass() const { return render_pass_; }


    // -------------------- Post Initialization -------------------
    void BeginRenderPass(const vk::CommandBuffer& command_buffer,
                         const VulkanPipeline& pipeline,
                         const vk::Queue& graphics_queue,
                         const vk::Queue& presentation_queue) const;

private:
    // -------------------- Type Definitions -------------------
    typedef std::vector<vk::ShaderModule> ShaderModuleVector;
    typedef std::vector<vk::Framebuffer> FramebufferVector;

    // -------------------- Immutable Initialization -------------------
    static vk::RenderPass InitializeRenderPass(const vk::Device& device, vk::SampleCountFlagBits num_samples, vk::Format image_format, vk::Format depth_format);
    static PipelineShaderStages InitializeShaderStages(const vk::Device& device);
    static ShaderModuleVector InitializeShaderModules(const PipelineShaderStages& shader_stages);
    static FramebufferVector InitializeFramebuffers(const vk::Device& device,
                                                    const vk::ImageView& depth_image_view,
                                                    const vk::RenderPass& render_pass,
                                                    const uint32_t width,
                                                    const uint32_t height,
                                                    const VulkanSwapchainDeprecated::SwapchainImageVector& swapchain_imge);
    static VertexBuffer InitializeVertexBuffer(const vk::Device& device, const VulkanGPU& gpu);
    static vk::VertexInputBindingDescription InitializeVertexInputBindingDescription();
    static VertexInputAttributeDescriptions InitializeVertexInputAttributeDescriptions();
    static vk::Semaphore InitializeImageAcquiredSemaphore(const vk::Device& device);
    static uint32_t InitializeCurrentSwapchainImageIndex(const vk::Device& device, const vk::SwapchainKHR& swapchain, const vk::Semaphore& image_acquired_semaphore);

    // -------------------- Utility -------------------
    // Reads the Spir-V bytecode into memory with proper data alignment
    static std::vector<uint32_t> ReadSpirV(const std::string file_name);

    // Begin the command buffer
    static void ExecuteBeginCommandBuffer(const vk::CommandBuffer& command_buffer);

    // -------------------- Data Members -------------------
    // The logical device responsible for resource allocation and cleanup
    const vk::Device& device_;

    // A reference to the swapchain manager
    const VulkanSwapchainDeprecated& swapchain_manager_;

    // The underlying render pass object
    vk::RenderPass render_pass_;

    // The shader stages which will be used in the pipeline
    // TODO Move this over to pipeline class?
    PipelineShaderStages pipeline_shader_stages_;

    // The shader module we will use in the render pass
    ShaderModuleVector shader_modules_;

    // The framebuffers we will use in the render pass
    FramebufferVector framebuffers_;

    // The vertex buffer where we store the geometry of the object(s)
    // we wish to render. This memory is both CPU and GPU visible.
    VertexBuffer vertex_buffer_;

    // Describes the data arrangement to the GPU
    // TODO These could probably be grouped into their own data structure
    vk::VertexInputBindingDescription vertex_input_binding_description_;
    VertexInputAttributeDescriptions vertex_input_attribute_descriptions_;

    // The semaphore object
    vk::Semaphore image_acquired_semaphore_;

    // The index of the swapchain image which is currently being rendered
    uint32_t current_swapchain_image_index_;

    // For logging
    static const std::string kWho;
};

}
}
