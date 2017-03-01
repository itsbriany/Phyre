#include "vulkan_pipeline.h"
#include "vulkan_errors.h"
#include "command_buffer_manager.h"

const std::string Phyre::Graphics::VulkanPipeline::kWho = "[VulkanPipeline]";

Phyre::Graphics::VulkanPipeline::VulkanPipeline(const vk::Device& device,
                                                const VulkanSwapchain& swapchain_manager,
                                                const VulkanMemoryManager& memory_manager,
                                                const CommandBufferManager& command_buffer_manager,
                                                const vk::Queue& graphics_queue,
                                                const vk::Queue& presentation_queue) :
    device_(device),
    graphics_queue_(graphics_queue),
    presentation_queue_(presentation_queue),
    swapchain_manager_(swapchain_manager),
    memory_manager_(memory_manager),
    command_buffer_manager_(command_buffer_manager),
    descriptor_set_count_(1),
    descriptor_set_layout_(InitializeDescriptorSetLayout(device)),
    descriptor_pool_(InitializeDescriptorPool(device)),
    pipeline_layout_(InitializePipelineLayout(device, descriptor_set_layout_, descriptor_set_count_)),
    render_pass_(device_, swapchain_manager_, memory_manager_),
    descriptor_set_(InitializeDescriptorSet(device, descriptor_pool_, descriptor_set_count_, descriptor_set_layout_, render_pass_, memory_manager.uniform_buffer())),
    pipeline_(InitializeGraphicsPipeline(device_, render_pass_, pipeline_layout_))
{
    Logging::trace("Instantiated", kWho);
    render_pass_.BeginRenderPass(command_buffer_manager_.command_buffer(), *this, graphics_queue, presentation_queue);
}

Phyre::Graphics::VulkanPipeline::~VulkanPipeline() {   
    device_.destroyPipeline(pipeline_);
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
                                                                           const vk::DescriptorSetLayout& descriptor_set_layout,
                                                                           const VulkanRenderPass& render_pass,
                                                                           const VulkanUniformBuffer& uniform_buffer) {
    vk::DescriptorSetAllocateInfo allocate_info;
    allocate_info.setDescriptorPool(descriptor_pool);
    allocate_info.setDescriptorSetCount(descriptor_set_count);
    allocate_info.setPSetLayouts(&descriptor_set_layout);

    vk::DescriptorSet descriptor_set;
    vk::Result result = device.allocateDescriptorSets(&allocate_info, &descriptor_set);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to create descriptor set", kWho);
    }

    UpdateDescriptorSet(device, descriptor_set, uniform_buffer.descriptor_buffer_info());
    return descriptor_set;
}

void
Phyre::Graphics::VulkanPipeline::UpdateDescriptorSet(const vk::Device& device, 
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

vk::Pipeline Phyre::Graphics::VulkanPipeline::InitializeGraphicsPipeline(const vk::Device& device,
                                                                         const VulkanRenderPass& render_pass,
                                                                         const vk::PipelineLayout& pipeline_layout) {
    // Dynamic State
    // These states will be added dynamically
    std::vector<vk::DynamicState> dynamic_states;

    // Vertex Input State
    // Let the pipeline know about the fomat and arrangement of the data
    vk::PipelineVertexInputStateCreateInfo vertex_input_state_create_info;
    vertex_input_state_create_info.setVertexBindingDescriptionCount(1);
    vertex_input_state_create_info.setPVertexBindingDescriptions(&render_pass.vertex_input_binding_description());
    vertex_input_state_create_info.setVertexAttributeDescriptionCount(render_pass.vertex_input_attribute_descriptions().size());
    vertex_input_state_create_info.setPVertexAttributeDescriptions(render_pass.vertex_input_attribute_descriptions().data());

    // Vertex Input Assembly State
    // Declares how vertices form the geometry we want to draw
    // In this example, we will say that every three vertices will draw a triangle
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_create_info;
    input_assembly_state_create_info.setPrimitiveRestartEnable(false);
    input_assembly_state_create_info.setTopology(vk::PrimitiveTopology::eTriangleList);

    // Rasterization state
    // Configures rasterization operations in the GPU
    vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info;
    rasterization_state_create_info.setPolygonMode(vk::PolygonMode::eFill);
    rasterization_state_create_info.setCullMode(vk::CullModeFlagBits::eBack);
    rasterization_state_create_info.setFrontFace(vk::FrontFace::eClockwise);
    rasterization_state_create_info.setDepthClampEnable(true);
    rasterization_state_create_info.setRasterizerDiscardEnable(false);
    rasterization_state_create_info.setDepthBiasEnable(false);
    rasterization_state_create_info.setDepthBiasConstantFactor(0);
    rasterization_state_create_info.setDepthBiasClamp(0);
    rasterization_state_create_info.setDepthBiasSlopeFactor(0);
    rasterization_state_create_info.setLineWidth(1.0f);

    // Color blend state
    vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info;

    std::array<vk::PipelineColorBlendAttachmentState, 1> color_blend_attachment_states;
    vk::ColorComponentFlags color_write_mask(vk::ColorComponentFlagBits::eR |
                                             vk::ColorComponentFlagBits::eG |
                                             vk::ColorComponentFlagBits::eB |
                                             vk::ColorComponentFlagBits::eA);
    color_blend_attachment_states[0].setColorWriteMask(color_write_mask);
    color_blend_attachment_states[0].setBlendEnable(false); // We disable this because we do not really care about blending for this example
    color_blend_attachment_states[0].setAlphaBlendOp(vk::BlendOp::eAdd);
    color_blend_attachment_states[0].setColorBlendOp(vk::BlendOp::eAdd);
    color_blend_attachment_states[0].setSrcAlphaBlendFactor(vk::BlendFactor::eZero); // We will not be blending in this example
    color_blend_attachment_states[0].setSrcColorBlendFactor(vk::BlendFactor::eZero);
    color_blend_attachment_states[0].setDstAlphaBlendFactor(vk::BlendFactor::eZero);
    color_blend_attachment_states[0].setDstColorBlendFactor(vk::BlendFactor::eZero);

    color_blend_state_create_info.setAttachmentCount(color_blend_attachment_states.size());
    color_blend_state_create_info.setPAttachments(color_blend_attachment_states.data());
    color_blend_state_create_info.setLogicOpEnable(false);
    color_blend_state_create_info.setLogicOp(vk::LogicOp::eNoOp);
    color_blend_state_create_info.setBlendConstants({ 1.0f, 1.0f, 1.0f, 1.0f });

    // Viewport State
    // For this example, the viewport and scissorts will be set dynamically via command buffers
    vk::PipelineViewportStateCreateInfo viewport_state_create_info;
    viewport_state_create_info.setViewportCount(1);
    dynamic_states.emplace_back(vk::DynamicState::eViewport);

    viewport_state_create_info.setScissorCount(1);
    dynamic_states.emplace_back(vk::DynamicState::eScissor);

    viewport_state_create_info.setPViewports(nullptr);
    viewport_state_create_info.setPScissors(nullptr);

    // Depth Stencil State
    vk::PipelineDepthStencilStateCreateInfo depth_stencil_state_create_info;
    depth_stencil_state_create_info.setDepthTestEnable(true);
    depth_stencil_state_create_info.setDepthWriteEnable(true);
    depth_stencil_state_create_info.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
    depth_stencil_state_create_info.setDepthBoundsTestEnable(false);
    depth_stencil_state_create_info.setMinDepthBounds(0);
    depth_stencil_state_create_info.setMaxDepthBounds(0);
    depth_stencil_state_create_info.setStencilTestEnable(false);
    
    vk::StencilOpState back_state;
    back_state.setFailOp(vk::StencilOp::eKeep);
    back_state.setPassOp(vk::StencilOp::eKeep);
    back_state.setCompareOp(vk::CompareOp::eAlways);
    back_state.setCompareMask(0);
    back_state.setReference(0);
    back_state.setDepthFailOp(vk::StencilOp::eKeep);
    back_state.setWriteMask(0);
    depth_stencil_state_create_info.setFront(back_state); // TODO Front = back?

    // Multisample State
    // We will not be using multisampling in this example
    vk::PipelineMultisampleStateCreateInfo multisample_state_create_info;
    multisample_state_create_info.setPSampleMask(nullptr);
    multisample_state_create_info.setRasterizationSamples(vk::SampleCountFlagBits::e1);
    multisample_state_create_info.setSampleShadingEnable(false);
    multisample_state_create_info.setAlphaToCoverageEnable(false);
    multisample_state_create_info.setAlphaToOneEnable(false);
    multisample_state_create_info.setMinSampleShading(0);

    // Now set the dynamic state info
    vk::PipelineDynamicStateCreateInfo dynamic_state_create_info;
    dynamic_state_create_info.setDynamicStateCount(dynamic_states.size());
    dynamic_state_create_info.setPDynamicStates(dynamic_states.data());

    // Put all the pieces together
    vk::GraphicsPipelineCreateInfo graphics_pipeline_create_info;
    
    graphics_pipeline_create_info.setLayout(pipeline_layout);
    graphics_pipeline_create_info.setBasePipelineHandle(nullptr);
    graphics_pipeline_create_info.setBasePipelineIndex(0);
    graphics_pipeline_create_info.setPVertexInputState(&vertex_input_state_create_info);
    graphics_pipeline_create_info.setPInputAssemblyState(&input_assembly_state_create_info);
    graphics_pipeline_create_info.setPRasterizationState(&rasterization_state_create_info);
    graphics_pipeline_create_info.setPColorBlendState(&color_blend_state_create_info);
    graphics_pipeline_create_info.setPTessellationState(nullptr);
    graphics_pipeline_create_info.setPMultisampleState(&multisample_state_create_info);
    graphics_pipeline_create_info.setPDynamicState(&dynamic_state_create_info);
    graphics_pipeline_create_info.setPViewportState(&viewport_state_create_info);
    graphics_pipeline_create_info.setPDepthStencilState(&depth_stencil_state_create_info);
    graphics_pipeline_create_info.setPStages(render_pass.pipeline_shader_stages().data());
    graphics_pipeline_create_info.setStageCount(render_pass.pipeline_shader_stages().size());
    graphics_pipeline_create_info.setSubpass(0);
    graphics_pipeline_create_info.setRenderPass(render_pass.render_pass());

    vk::PipelineCache pipeline_cache;
    vk::Pipeline pipeline = device.createGraphicsPipeline(pipeline_cache, graphics_pipeline_create_info);
    return pipeline;
}
