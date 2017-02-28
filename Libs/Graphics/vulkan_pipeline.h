#pragma once
#include <vulkan.hpp>
#include "vulkan_render_pass.h"

namespace Phyre {
namespace Graphics {
class CommandBufferManager;

// For more information on how the shader explicitly references descriptors in the shader language,
// check out https://vulkan.lunarg.com/doc/sdk/1.0.39.1/windows/tutorial/html/08-init_pipeline_layout.html
class VulkanPipeline {
public:
    typedef std::array<vk::WriteDescriptorSet, 2> WriteDescriptorSets;

    explicit VulkanPipeline(const vk::Device& device,
                            const SwapchainManager& swapchain_manager,
                            const VulkanMemoryManager& memory_manager, 
                            const CommandBufferManager& command_buffer_manager,
                            const vk::Queue& graphics_queue,
                            const vk::Queue& presentation_queue);
    ~VulkanPipeline();

    const vk::PipelineLayout& pipeline_layout() const { return pipeline_layout_; }
    const vk::Pipeline& pipeline() const { return pipeline_; }
    const vk::DescriptorSet& descriptor_set() const { return descriptor_set_; }

private:
    static vk::DescriptorSetLayout InitializeDescriptorSetLayout(const vk::Device& device);
    static vk::DescriptorPool InitializeDescriptorPool(const vk::Device& device);
    static vk::PipelineLayout InitializePipelineLayout(const vk::Device& device,
                                                       const vk::DescriptorSetLayout& descriptor_set_layout,
                                                       uint32_t descriptor_sets_layout_count);
    static vk::DescriptorSet InitializeDescriptorSet(const vk::Device& device,
                                                     const vk::DescriptorPool& descriptor_pool,
                                                     uint32_t descriptor_set_count,
                                                     const vk::DescriptorSetLayout& descriptor_set_layout,
                                                     const VulkanRenderPass& render_pass,
                                                     const VulkanUniformBuffer& uniform_buffer);
    static void UpdateDescriptorSet(const vk::Device& device,
                                             const vk::DescriptorSet& descriptor_set,
                                             const vk::DescriptorBufferInfo& descriptor_buffer_info);

    static vk::Pipeline InitializeGraphicsPipeline(const vk::Device& device,
                                                   const VulkanRenderPass& render_pass,
                                                   const vk::PipelineLayout& pipeline_layout);

    // A reference to the logical device
    const vk::Device& device_;

    // A reference to the graphics queue
    const vk::Queue graphics_queue_;

    // A reference to the presentation queue
    const vk::Queue presentation_queue_;

    // A reference to the swapchain manager
    const SwapchainManager& swapchain_manager_;

    // A reference to the memory manager
    const VulkanMemoryManager& memory_manager_;

    // A reference to the command buffer manager
    const CommandBufferManager& command_buffer_manager_;

    // Determines how many descriptor sets we will be using
    uint32_t descriptor_set_count_;

    // Describes the content of a list of descriptor sets
    // Informs the GPU how the data contained in the uniform buffer
    // is mapped to the shader program's uniform variables
    vk::DescriptorSetLayout descriptor_set_layout_;

    // The pool of descriptors
    vk::DescriptorPool descriptor_pool_;

    // Holds a list of descriptor set layouts
    vk::PipelineLayout pipeline_layout_;

    // The render pass
    VulkanRenderPass render_pass_;

    // The real allocated descriptor set
    vk::DescriptorSet descriptor_set_;

    // ----------------------------- Pipeline States ----------------------
    
    // The graphics pipeline
    vk::Pipeline pipeline_;

    // For logging
    static const std::string kWho;
};

}
}
