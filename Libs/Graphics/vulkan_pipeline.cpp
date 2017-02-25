#include "vulkan_pipeline.h"
#include "vulkan_errors.h"

const std::string Phyre::Graphics::VulkanPipeline::kWho = "[VulkanPipeline]";

Phyre::Graphics::VulkanPipeline::VulkanPipeline(const vk::Device& device) : 
    device_(device),
    descriptor_set_count_(1),
    descriptor_set_layout_(InitializeDescriptorSetLayout(device)),
    descriptor_pool_(InitializeDescriptorPool(device)),
    pipeline_layout_(InitializePipelineLayout(device, descriptor_set_layout_, descriptor_set_count_)),
    descriptor_set_(InitializeDescriptorSet(device, descriptor_pool_, descriptor_set_count_, descriptor_set_layout_))
{
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::VulkanPipeline::~VulkanPipeline() {    
    device_.destroyPipelineLayout(pipeline_layout_);
    device_.destroyDescriptorPool(descriptor_pool_);
    device_.destroyDescriptorSetLayout(descriptor_set_layout_);
    Logging::trace("Destroyed", kWho);
}

vk::DescriptorSetLayout Phyre::Graphics::VulkanPipeline::InitializeDescriptorSetLayout(const vk::Device& device) {
    // Describes the descriptor set
    vk::DescriptorSetLayoutBinding descriptor_set_layout_binding;
    descriptor_set_layout_binding.setBinding(0); // We choose 0 because we only have one descriptor set which is indexed at 0
    descriptor_set_layout_binding.setDescriptorType(vk::DescriptorType::eUniformBuffer); // What kind of data will this descriptor set hold?
    descriptor_set_layout_binding.setDescriptorCount(1);
    descriptor_set_layout_binding.setStageFlags(vk::ShaderStageFlagBits::eVertex); // Which pipeline shader stages may access this binding?
    descriptor_set_layout_binding.setPImmutableSamplers(nullptr);

    vk::DescriptorSetLayoutCreateInfo create_info;
    create_info.setBindingCount(1);
    create_info.setPBindings(&descriptor_set_layout_binding);

    vk::DescriptorSetLayout descriptor_set_layout;
    vk::Result result = device.createDescriptorSetLayout(&create_info, nullptr, &descriptor_set_layout);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to create descriptor set layout", kWho);
    }
    return descriptor_set_layout;
}

vk::DescriptorPool Phyre::Graphics::VulkanPipeline::InitializeDescriptorPool(const vk::Device& device) {
    vk::DescriptorPoolSize descriptor_pool_size;
    descriptor_pool_size.setType(vk::DescriptorType::eUniformBuffer);
    descriptor_pool_size.setDescriptorCount(1);

    vk::DescriptorPoolCreateInfo create_info;
    create_info.setMaxSets(1);
    create_info.setPoolSizeCount(1);
    create_info.setPPoolSizes(&descriptor_pool_size);

    vk::DescriptorPool descriptor_pool;
    vk::Result result = device.createDescriptorPool(&create_info, nullptr, &descriptor_pool);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Could not instantiate descriptor pool", kWho);
    }
    return descriptor_pool;
}

vk::PipelineLayout Phyre::Graphics::VulkanPipeline::InitializePipelineLayout(const vk::Device& device,
                                                                             const vk::DescriptorSetLayout& descriptor_set_layout,
                                                                             uint32_t descriptor_set_layouts_count) {
    vk::PipelineLayoutCreateInfo create_info;
    create_info.setPushConstantRangeCount(0);
    create_info.setPPushConstantRanges(nullptr);
    create_info.setSetLayoutCount(descriptor_set_layouts_count);
    create_info.setPSetLayouts(&descriptor_set_layout);

    vk::PipelineLayout pipeline_layout;
    vk::Result result = device.createPipelineLayout(&create_info, nullptr, &pipeline_layout);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to create pipeline layout", kWho);
    }
    return pipeline_layout;
}

vk::DescriptorSet Phyre::Graphics::VulkanPipeline::InitializeDescriptorSet(const vk::Device& device,
                                                                           const vk::DescriptorPool& descriptor_pool,
                                                                           uint32_t descriptor_set_count,
                                                                           const vk::DescriptorSetLayout& descriptor_set_layout) {
    vk::DescriptorSetAllocateInfo allocate_info;
    allocate_info.setDescriptorPool(descriptor_pool);
    allocate_info.setDescriptorSetCount(descriptor_set_count);
    allocate_info.setPSetLayouts(&descriptor_set_layout);

    vk::DescriptorSet descriptor_set;
    vk::Result result = device.allocateDescriptorSets(&allocate_info, &descriptor_set);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to create descriptor set", kWho);
    }
    return descriptor_set;
}

void
Phyre::Graphics::VulkanPipeline::InitializeWriteDescriptorSet(const vk::Device& device, 
                                                              const vk::DescriptorSet& descriptor_set,
                                                              const vk::DescriptorBufferInfo& descriptor_buffer_info) {
    // Copy the vk::DescriptorBufferInfo to the descriptor likely residing in device memory
    WriteDescriptorSets writes;
    writes[0] = vk::WriteDescriptorSet();
    writes[0].setDstSet(descriptor_set);
    writes[0].setDescriptorCount(1);
    writes[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
    writes[0].setPBufferInfo(&descriptor_buffer_info);
    writes[0].setDstArrayElement(0);
    writes[0].setDstBinding(0);

    // The byte layout of the actual descriptor is driver implementation specific,
    // Vulkan abstracts this
    uint32_t descriptor_write_count = 1;
    uint32_t descriptor_copy_count = 0;
    vk::CopyDescriptorSet* copy_descriptor_set = nullptr;
    device.updateDescriptorSets(descriptor_write_count, writes.data(), descriptor_copy_count, copy_descriptor_set);
}
