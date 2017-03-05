#include <fstream>
#include "draw_cube.h"
#include "geometry.h"
#include "logging.h"
#include "vulkan_memory_manager.h"

int main(int argc, const char* argv[]) {
    Phyre::Graphics::DrawCube app;
    app.Start();
    return 0;
}

const std::string Phyre::Graphics::DrawCube::kWho = "[DrawCube]";

Phyre::Graphics::DrawCube::DrawCube() : 
    instance_(),
    debugger_(instance_),
    p_active_gpu_(nullptr),
    p_window_(nullptr),
    p_device_(nullptr),
    p_swapchain_(nullptr),
    p_uniform_buffer_(nullptr),
    p_render_pass_(nullptr) {
    Logging::trace("Instantiated", kWho);
}

void Phyre::Graphics::DrawCube::DestroyShaderModules() {
    for (const auto& shader : shader_stages_) {
        p_device_->get().destroyShaderModule(shader.module);
    }
}

void Phyre::Graphics::DrawCube::DestroyVertexBuffer() const {
    p_device_->get().destroyBuffer(vertex_buffer_.buffer);
    p_device_->get().freeMemory(vertex_buffer_.memory);
}

void Phyre::Graphics::DrawCube::Start() {
#ifndef NDEBUG
    StartDebugger();
#endif
    LoadGPUs();
    LoadWindow(500, 500);
    // TODO On other platforms, we may need to initialize a connection to the window

    LoadDevice();
    LoadCommandPool();
    LoadCommandBuffers();
    LoadSwapchain();
    LoadShaderModules();
    LoadVertexBuffer();
    LoadUniformBuffer();
    LoadDescriptorPool();
    LoadDescriptorSets();
    LoadRenderPass();
    LoadPipelineCache();
    LoadPipelineLayout();
    LoadPipeline();
    LoadVulkanFence();
    Draw();
}

Phyre::Graphics::DrawCube::~DrawCube() {
    p_device_->get().destroyFence(fence_);
    p_device_->get().destroyPipeline(pipeline_);
    p_device_->get().destroyPipelineLayout(pipeline_layout_);
    p_device_->get().destroyPipelineCache(pipeline_cache_);
    p_device_->get().destroyDescriptorSetLayout(descriptor_set_layout_);
    p_device_->get().destroyDescriptorPool(descriptor_pool_);
    delete p_render_pass_;
    delete p_uniform_buffer_;
    DestroyVertexBuffer();
    DestroyShaderModules();
    delete p_swapchain_;
    p_device_->get().destroyCommandPool(command_pool_);
    delete p_window_;
    delete p_device_;
    Logging::trace("Destroyed", kWho);
}

void Phyre::Graphics::DrawCube::StartDebugger() {
    debugger_.InitializeDebugReport();
}

void Phyre::Graphics::DrawCube::LoadGPUs() {
    gpus_.clear();
    std::vector<vk::PhysicalDevice> physical_device_vector = instance_.get().enumeratePhysicalDevices();
    for (const vk::PhysicalDevice& physical_device : physical_device_vector) {
        const vk::PhysicalDeviceProperties properties = physical_device.getProperties();
        std::ostringstream oss;
        oss << "Found device: " << properties.deviceName;
        Logging::info(oss.str(), kWho);
        gpus_.emplace_back(VulkanGPU(physical_device));
    }
    if (gpus_.empty()) {
        std::string error_message = "Could not locate any GPUs";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }
    p_active_gpu_ = &gpus_[0];
}

void Phyre::Graphics::DrawCube::LoadWindow(uint32_t width, uint32_t height) {
    p_window_ = new VulkanWindow(width, height, instance_, *p_active_gpu_);
}

void Phyre::Graphics::DrawCube::LoadDevice() {
    p_device_ = new VulkanDevice(*p_active_gpu_, *p_window_);
}

void Phyre::Graphics::DrawCube::LoadCommandPool() {
    /**
    * Command buffers reside in command buffer pools.
    * This is necessary for allocating command buffers
    * because memory is coarsly allocated in large chunks between the CPU and GPU.
    */
    vk::CommandPoolCreateInfo command_pool_create_info(vk::CommandPoolCreateFlags(), p_device_->graphics_queue_family_index());
    command_pool_ = p_device_->get().createCommandPool(command_pool_create_info);
}

void Phyre::Graphics::DrawCube::LoadCommandBuffers() {
    vk::CommandBufferAllocateInfo command_buffer_allocate_info;
    uint32_t command_buffer_count = 1;
    command_buffer_allocate_info.setCommandPool(command_pool_);
    command_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);
    command_buffer_allocate_info.setCommandBufferCount(command_buffer_count);
    
    command_buffers_ = p_device_->get().allocateCommandBuffers(command_buffer_allocate_info);
}

void Phyre::Graphics::DrawCube::ExecuteBeginCommandBuffer(size_t command_buffer_index) {
    if (command_buffer_index > command_buffers_.size()) {
        Logging::error("Failed to begin command buffer: index out of bounds", kWho);
        return;
    }

    vk::CommandBufferBeginInfo info;
    info.setPInheritanceInfo(nullptr);
    command_buffers_[command_buffer_index].begin(&info);
    Logging::trace("Command buffer begin", kWho);
}

void Phyre::Graphics::DrawCube::LoadSwapchain() {
    p_swapchain_ = new VulkanSwapchain(*p_device_, *p_window_);
}

void Phyre::Graphics::DrawCube::LoadPipeline() {
    // Dynamic State
    // These states will be added dynamically
    std::vector<vk::DynamicState> dynamic_states;
    
    // Vertex Input State
    // Let the pipeline know about the fomat and arrangement of the data
    vk::PipelineVertexInputStateCreateInfo vertex_input_state_create_info;
    vertex_input_state_create_info.setVertexBindingDescriptionCount(1);
    vertex_input_state_create_info.setPVertexBindingDescriptions(&vertex_input_binding_description_);
    vertex_input_state_create_info.setVertexAttributeDescriptionCount(vertex_input_attributes_.size());
    vertex_input_state_create_info.setPVertexAttributeDescriptions(vertex_input_attributes_.data());
    
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
        
    graphics_pipeline_create_info.setLayout(pipeline_layout_);
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
    graphics_pipeline_create_info.setPStages(shader_stages_.data());
    graphics_pipeline_create_info.setStageCount(shader_stages_.size());
    graphics_pipeline_create_info.setSubpass(0);
    graphics_pipeline_create_info.setRenderPass(p_render_pass_->get());
    
    pipeline_ = p_device_->get().createGraphicsPipeline(pipeline_cache_, graphics_pipeline_create_info);
}

void Phyre::Graphics::DrawCube::LoadVulkanFence() {
    // Create a fence which we can use to know when the GPU is done rendering
    // This is so that we can be guaranteed to display fully rendered images
    vk::FenceCreateInfo fence_create_info;
    fence_ = p_device_->get().createFence(fence_create_info);
}

void Phyre::Graphics::DrawCube::Draw() {
    // We cannot bind the vertex buffer until we begin a renderpass
    std::array<vk::ClearValue, 2> clear_values;
    vk::ClearColorValue color;
    color.setFloat32({ 0.2f, 0.2f, 0.2f, 0.2f });
    clear_values[0].setColor(color);

    vk::ClearDepthStencilValue depth_stencil;
    depth_stencil.setDepth(1.0f);
    depth_stencil.setStencil(0);
    clear_values[1].setDepthStencil(depth_stencil);

    // Prepare the begin render pass
    vk::RenderPassBeginInfo begin_info;
    begin_info.setRenderPass(p_render_pass_->get());
    begin_info.setFramebuffer(p_render_pass_->current_frame_buffer());

    vk::Offset2D offset;
    vk::Extent2D extent(p_swapchain_->image_width(), p_swapchain_->image_height());
    vk::Rect2D render_area(offset, extent);
    begin_info.setRenderArea(render_area);

    begin_info.setClearValueCount(clear_values.size());
    begin_info.setPClearValues(clear_values.data());

    // We can only connect vertex buffers between the begin and end stages of a render pass
    // Begin the render pass stage
    uint32_t command_buffer_index = 0;
    const vk::CommandBuffer& command_buffer = command_buffers_[command_buffer_index];
    ExecuteBeginCommandBuffer(command_buffer_index);
    command_buffer.beginRenderPass(&begin_info, vk::SubpassContents::eInline);

    // Bind vertex buffers
    uint32_t start_binding = 0; // We will start our binding at offset 0
    uint32_t binding_count = 1; // We only have one vertex buffer
    std::array<vk::DeviceSize, 1> offsets = { 0 };
    command_buffer.bindVertexBuffers(start_binding, binding_count, &vertex_buffer_.buffer, offsets.data());

    // Bind graphics pipeline
    // Note that it is also possible to create multiple graphics pipelines and switch between them with a single
    // command buffer
    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_);

    // Bind the descriptor sets
    // Now the pipeline will know how to find its input data like the MVP transform
    uint32_t first_set = 0;
    command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout_, first_set, 1, descriptor_sets_.data(), 0, nullptr);
    // command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.pipeline_layout(), first_set, pipeline.descriptor_set(), nullptr);

    // We bind the viewports and scissors dynamically because earlier we specified that they would be dynamically
    // loaded from a command buffer.
    // A reason to keep these dynamic is becuase most applications need to update these values if the window size
    // changes during execution. This avoids having to rebuild the pipeline when the window size changes.
    // Bind the viewports
    vk::Viewport viewport;
    viewport.setHeight(p_swapchain_->image_height());
    viewport.setWidth(p_swapchain_->image_width());
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);
    viewport.setX(0);
    viewport.setY(0);

    uint32_t first_viewport = 0;
    command_buffer.setViewport(first_viewport, viewport);

    // Bind the scissors
    vk::Rect2D scissor;
    vk::Extent2D scissor_extent;
    scissor_extent.setWidth(p_swapchain_->image_width());
    scissor_extent.setHeight(p_swapchain_->image_height());

    vk::Offset2D scissor_offset;
    scissor_offset.setX(0);
    scissor_offset.setY(0);

    scissor.setExtent(scissor_extent);
    scissor.setOffset(scissor_offset);

    uint32_t first_scissor = 0;
    command_buffer.setScissor(first_scissor, scissor);

    // Issue a draw command to tell the GPU to send the vertices into the pipeline
    // and finish the render pass
    uint32_t vertex_count = 12 * 3; // 36 vertices because we have 12 triangles for this example
    uint32_t instance_count = 1;
    uint32_t first_vertex = 0;
    uint32_t first_instance = 0;
    command_buffer.draw(vertex_count, instance_count, first_vertex, first_instance);

    // End render pass
    command_buffer.endRenderPass();

    // End command buffer
    command_buffer.end();

    // Finally, submit the command buffer
    std::array<vk::CommandBuffer, 1> command_buffers { command_buffer };
    vk::PipelineStageFlags pipeline_stage_flags(vk::PipelineStageFlagBits::eBottomOfPipe); // The final stage in the pipeline where the commands finish execution

    vk::SubmitInfo submit_info;
    submit_info.setWaitSemaphoreCount(1);
    submit_info.setPWaitSemaphores(&p_render_pass_->image_acquired_semaphore()); // This forces a wait until the image is ready before drawing so that we know when the swapchain image is available
                                                                // When the GPU is done executing commands, it signals the fence to indicate that the drawing is complete
    submit_info.setPWaitDstStageMask(&pipeline_stage_flags);
    submit_info.setCommandBufferCount(command_buffers.size());
    submit_info.setPCommandBuffers(command_buffers.data());
    submit_info.setSignalSemaphoreCount(0);
    submit_info.setPSignalSemaphores(nullptr);

    // Submit to the queue
    p_device_->graphics_queue().submit(1, &submit_info, fence_);

    // Now present the image to the window
    vk::PresentInfoKHR present_info;
    present_info.setSwapchainCount(1);
    present_info.setPSwapchains(&p_swapchain_->swapchain());
    present_info.setPImageIndices(&p_render_pass_->swapchain_current_index());
    present_info.setWaitSemaphoreCount(0);
    present_info.setPWaitSemaphores(nullptr);
    present_info.setPResults(nullptr);

    // Make sure the command buffer finished before presenting
    vk::Result result = vk::Result::eTimeout;
    do {
        bool wait_all = true;
        uint64_t timeout = 100000000; // The amount of time in nanoseconds for a command buffer to complete
        result = p_device_->get().waitForFences(1, &fence_, wait_all, timeout);
    } while (result == vk::Result::eTimeout);


    p_device_->presentation_queue().presentKHR(&present_info);
    Sleep(1000);
}

void Phyre::Graphics::DrawCube::LoadShaderModules() {
    // This is where the SPIR-V intermediate bytecode is located
    // std::string resource_directory("GraphicsTestResources/");
    std::string vertex_file_name("vertices.spv");
    std::vector<uint32_t> vertex_shader_bytecode(ReadSpirV(vertex_file_name));

    std::string fragment_file_name("fragments.spv");
    std::vector<uint32_t> fragment_shader_bytecode(ReadSpirV(fragment_file_name));

    // We will be creating two pipeline shader stages: One for vertices, and one for fragments
    // Create the vertex shader module
    uint32_t vertex = 0;
    uint32_t fragment = 1;
    shader_stages_[vertex].setPSpecializationInfo(nullptr);
    shader_stages_[vertex].setStage(vk::ShaderStageFlagBits::eVertex);
    shader_stages_[vertex].setPName("main");

    vk::ShaderModuleCreateInfo vertex_shader_module_create_info;
    vertex_shader_module_create_info.setCodeSize(vertex_shader_bytecode.size() * sizeof(uint32_t));
    vertex_shader_module_create_info.setPCode(vertex_shader_bytecode.data());

    shader_stages_[vertex].module = p_device_->get().createShaderModule(vertex_shader_module_create_info);

    // Create the fragment shader module
    shader_stages_[fragment].setPSpecializationInfo(nullptr);
    shader_stages_[fragment].setStage(vk::ShaderStageFlagBits::eFragment);
    shader_stages_[fragment].setPName("main");

    vk::ShaderModuleCreateInfo fragment_shader_module_create_info;
    fragment_shader_module_create_info.setCodeSize(fragment_shader_bytecode.size() * sizeof(uint32_t));
    fragment_shader_module_create_info.setPCode(fragment_shader_bytecode.data());

    shader_stages_[fragment].module = p_device_->get().createShaderModule(fragment_shader_module_create_info);
}

void Phyre::Graphics::DrawCube::LoadVertexBuffer() {
    vk::BufferCreateInfo info;
    vk::DeviceSize data_size = Geometry::kVertexBufferSolidFaceColorData.size() * sizeof(Geometry::Vertex);

    info.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
    info.setSize(data_size);
    info.setQueueFamilyIndexCount(0);
    info.setPQueueFamilyIndices(nullptr);
    info.setSharingMode(vk::SharingMode::eExclusive);
    vertex_buffer_.buffer = p_device_->get().createBuffer(info);

    vk::MemoryRequirements memory_requirements = p_device_->get().getBufferMemoryRequirements(vertex_buffer_.buffer);
    vk::MemoryAllocateInfo allocate_info;
    vk::MemoryPropertyFlags flags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    allocate_info.setAllocationSize(memory_requirements.size);
    if (!VulkanMemoryManager::CanFindMemoryTypeFromProperties(p_device_->gpu(), memory_requirements.memoryTypeBits, flags, allocate_info.memoryTypeIndex)) {
        Logging::fatal("Failed to get memory type from properties while allocating vertex buffer", kWho);
    }
    vertex_buffer_.memory = p_device_->get().allocateMemory(allocate_info);

    void *p_data = p_device_->get().mapMemory(vertex_buffer_.memory, 0, memory_requirements.size);
    memcpy(p_data, Geometry::kVertexBufferSolidFaceColorData.data(), data_size);
    p_device_->get().unmapMemory(vertex_buffer_.memory);
    p_device_->get().bindBufferMemory(vertex_buffer_.buffer, vertex_buffer_.memory, 0);

    uint32_t stride = sizeof(Geometry::Vertex);
    vertex_input_binding_description_.setBinding(0);
    vertex_input_binding_description_.setInputRate(vk::VertexInputRate::eVertex);
    vertex_input_binding_description_.setStride(sizeof(Geometry::Vertex));

    vertex_input_attributes_[0].setBinding(0);
    vertex_input_attributes_[0].setLocation(0);
    vertex_input_attributes_[0].setFormat(vk::Format::eR32G32B32A32Sfloat);
    vertex_input_attributes_[0].setOffset(0);

    vertex_input_attributes_[1].setBinding(0);
    vertex_input_attributes_[1].setLocation(1);
    vertex_input_attributes_[1].setFormat(vk::Format::eR32G32B32A32Sfloat);
    vertex_input_attributes_[1].setOffset(16);
}

void Phyre::Graphics::DrawCube::LoadUniformBuffer() {
    p_uniform_buffer_ = new VulkanUniformBuffer(*p_device_);
}

void Phyre::Graphics::DrawCube::LoadRenderPass() {
    p_render_pass_ = new VulkanRenderPass(*p_device_, *p_swapchain_);
}

void Phyre::Graphics::DrawCube::LoadDescriptorPool() {
    vk::DescriptorPoolSize descriptor_pool_size;
    descriptor_pool_size.setType(vk::DescriptorType::eUniformBuffer);
    descriptor_pool_size.setDescriptorCount(1);

    vk::DescriptorPoolCreateInfo create_info;
    create_info.setMaxSets(1);
    create_info.setPoolSizeCount(1);
    create_info.setPPoolSizes(&descriptor_pool_size);

    descriptor_pool_ = p_device_->get().createDescriptorPool(create_info);
}

void Phyre::Graphics::DrawCube::LoadDescriptorSets() {
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

    descriptor_set_layout_ = p_device_->get().createDescriptorSetLayout(create_info);

    vk::DescriptorSetAllocateInfo allocate_info;
    allocate_info.setDescriptorPool(descriptor_pool_);
    allocate_info.setDescriptorSetCount(1);
    allocate_info.setPSetLayouts(&descriptor_set_layout_);

    descriptor_sets_ = p_device_->get().allocateDescriptorSets(allocate_info);

    // Copy the vk::DescriptorBufferInfo to the descriptor likely residing in device memory
    std::array<vk::WriteDescriptorSet, 1> writes;
    writes[0] = vk::WriteDescriptorSet();
    writes[0].setDstSet(descriptor_sets_[0]);
    writes[0].setDescriptorCount(1);
    writes[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
    writes[0].setPBufferInfo(&p_uniform_buffer_->descriptor_buffer_info());
    writes[0].setDstArrayElement(0);
    writes[0].setDstBinding(0);

    // The byte layout of the actual descriptor is driver implementation specific,
    // Vulkan abstracts this
    uint32_t descriptor_write_count = 1;
    uint32_t descriptor_copy_count = 0;
    vk::CopyDescriptorSet* copy_descriptor_set = nullptr;
    p_device_->get().updateDescriptorSets(descriptor_write_count, writes.data(), descriptor_copy_count, copy_descriptor_set);
}

void Phyre::Graphics::DrawCube::LoadPipelineCache() {
    vk::PipelineCacheCreateInfo info; // Defaults should work for this example
    pipeline_cache_ = p_device_->get().createPipelineCache(info);
}

void Phyre::Graphics::DrawCube::LoadPipelineLayout() {
    if (!p_device_) {
        Logging::error("Failed to load pipeline layout: No logical device", kWho);
        return;
    }
    vk::PipelineLayoutCreateInfo create_info;
    create_info.setPushConstantRangeCount(0);
    create_info.setPPushConstantRanges(nullptr);
    create_info.setSetLayoutCount(1);
    create_info.setPSetLayouts(&descriptor_set_layout_);

    pipeline_layout_ = p_device_->get().createPipelineLayout(create_info);
}

std::vector<uint32_t> Phyre::Graphics::DrawCube::ReadSpirV(const std::string spirv_shader_file_name) {
    std::ifstream shader_file(spirv_shader_file_name, std::ios::binary);
    if (!shader_file) {
        std::ostringstream oss;
        oss << "Could not open " << spirv_shader_file_name;
        Logging::error(oss.str(), kWho);
        return std::vector<uint32_t>();
    }
    // get length of file:
    shader_file.seekg(0, shader_file.end);
    size_t length = shader_file.tellg();
    shader_file.seekg(0, shader_file.beg);

    // Spir-V byte alignment should represent the size of a uint32_t
    std::vector<uint32_t> vertex_shader_bytecode(length / sizeof(uint32_t));
    shader_file.read(reinterpret_cast<char*>(vertex_shader_bytecode.data()), length);
    std::ostringstream oss;
    oss << "Read " << vertex_shader_bytecode.size() << " SPIR-V data blocks";
    Logging::debug(oss.str(), kWho);
    shader_file.close();

    return vertex_shader_bytecode;
}