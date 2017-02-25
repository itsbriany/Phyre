#pragma once
#include <vulkan.hpp>

namespace Phyre {
namespace Graphics {

// For more information on how the shader explicitly references descriptors in the shader language,
// check out https://vulkan.lunarg.com/doc/sdk/1.0.39.1/windows/tutorial/html/08-init_pipeline_layout.html
class VulkanPipeline {
public:
    typedef std::array<vk::WriteDescriptorSet, 2> WriteDescriptorSets;

    explicit VulkanPipeline(const vk::Device& device);
    ~VulkanPipeline();


private:
    static vk::DescriptorSetLayout InitializeDescriptorSetLayout(const vk::Device& device);
    static vk::DescriptorPool InitializeDescriptorPool(const vk::Device& device);
    static vk::PipelineLayout InitializePipelineLayout(const vk::Device& device,
                                                       const vk::DescriptorSetLayout& descriptor_set_layout,
                                                       uint32_t descriptor_sets_layout_count);
    static vk::DescriptorSet InitializeDescriptorSet(const vk::Device& device,
                                                     const vk::DescriptorPool& descriptor_pool,
                                                     uint32_t descriptor_set_count,
                                                     const vk::DescriptorSetLayout& descriptor_set_layout);
    static void InitializeWriteDescriptorSet(const vk::Device& device,
                                             const vk::DescriptorSet& descriptor_set,
                                             const vk::DescriptorBufferInfo& descriptor_buffer_info);


    // A reference to the logical device
    const vk::Device& device_;

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

    // The real allocated descriptor set
    vk::DescriptorSet descriptor_set_;

    // ?
    WriteDescriptorSets write_descriptor_set_;

    // For logging
    static const std::string kWho;
};

}
}
