#include <chrono>
#include <fstream>

#include <glm/gtc/matrix_transform.inl>

#include <Graphics/geometry.h>
#include <Graphics/vulkan_utils.h>
#include <Graphics/uniform_buffer.h>
#include <Logging/logging.h>

#include "draw_cube.h"

void Run(int argc, const char* argv[]) {
    using Phyre::Graphics::DrawCube;
    std::shared_ptr<DrawCube> app = DrawCube::Create(argc, argv);

    while (app->Run()) {
        app->BeginRender();
        app->LogFPS();
        app->Draw();
        app->EndRender();
    }

    app->Stop();
}

int main(int argc, const char* argv[]) {
    Run(argc, argv);
    return 0;
}

const std::string Phyre::Graphics::DrawCube::kWho = "[DrawCube]";

Phyre::Graphics::DrawCube::DrawCube(int argc, const char* argv[]) :
    BaseClass(1),
    instance_(),
    debugger_(instance_),
    p_active_gpu_(nullptr),
    p_vk_window_(nullptr),
    p_device_(nullptr),
    p_swapchain_(nullptr),
    p_uniform_buffer_(nullptr),
    p_render_pass_(nullptr),
    p_provider_(nullptr),
    target_("DrawCube")
{
    if (argc < 2) {
        PHYRE_LOG(fatal, kWho) << "Please provide a phyre configuration file!";
        exit(EXIT_FAILURE);
    }
    std::string configuration = argv[1];
    p_provider_ = std::make_unique<Configuration::Provider>(configuration);

    LoadGraphics();
    PHYRE_LOG(trace, kWho) << "Instantiated";
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

void Phyre::Graphics::DrawCube::DestroyFramebuffers() {
    for (const auto& framebuffer : framebuffers_) {
        p_device_->get().destroyFramebuffer(framebuffer);
    }
}

void Phyre::Graphics::DrawCube::ReloadSwapchain() {
    p_device_->get().waitIdle();
    LoadSwapchain();
    LoadRenderPass();
    LoadFrameBuffers();

    // We don't need to create the graphics pipeline again because
    // we specified that the viewport and scissor would have dynamic state
}

void Phyre::Graphics::DrawCube::LoadGraphics() {
#ifndef NDEBUG
    StartDebugger();
#endif
    LoadGPUs();
    LoadWindow(500, 500, "Phyre");
    // TODO On other platforms, we may need to initialize a connection to the window

    LoadDevice();
    LoadSwapchain();
    LoadShaderModules();
    LoadVertexBuffer();
    LoadUniformBuffer();
    LoadDescriptorPool();
    LoadDescriptorSets();
    LoadRenderPass();
    LoadFrameBuffers();
    LoadPipelineCache();
    LoadPipelineLayout();
    LoadPipeline();
    LoadCommandPool();
    LoadCommandBuffers();
    LoadSemaphores();
}

bool Phyre::Graphics::DrawCube::Run() const {
    return p_vk_window_->Update();
}

std::shared_ptr<Phyre::Graphics::DrawCube> Phyre::Graphics::DrawCube::Create(int argc, const char* argv[]) {
    std::shared_ptr<DrawCube> app(new DrawCube(argc, argv));
    app->Bind(app->input_window());
    return app;
}

Phyre::Graphics::DrawCube::~DrawCube() {
    p_device_->get().destroySemaphore(render_finished_semaphore_);
    p_device_->get().destroyPipeline(pipeline_);
    p_device_->get().destroyPipelineLayout(pipeline_layout_);
    p_device_->get().destroyPipelineCache(pipeline_cache_);
    p_device_->get().destroyDescriptorSetLayout(descriptor_set_layout_);
    p_device_->get().destroyDescriptorPool(descriptor_pool_);
    DestroyFramebuffers();
    p_render_pass_.reset();
    p_uniform_buffer_.reset();
    DestroyVertexBuffer();
    DestroyShaderModules();
    p_swapchain_.reset();
    p_device_->get().destroyCommandPool(command_pool_);
    p_vk_window_.reset();
    delete p_device_;
    PHYRE_LOG(trace, kWho) << "Destroyed";
}

void Phyre::Graphics::DrawCube::OnFramebufferResize(int width, int height) {
    PHYRE_LOG(debug, kWho) << "Framebuffer dimensions: (" << width << "x" << height << ')';
    ReloadSwapchain();
}

void Phyre::Graphics::DrawCube::OnMousePositionUpdate(double x, double y) {
    Input::Cursor::Mode cursor_mode = CursorMode();
    if (cursor_mode == Input::Cursor::kDisabled) {
        if (MouseButton(Input::Mouse::kButton1) == Input::Action::kPressed) {
            PHYRE_LOG(debug, kWho) << "PRESSED: Mouse position: (" << x << ", " << y << ')';
            UpdateUniformBuffers();
        }
    }
}

void Phyre::Graphics::DrawCube::OnKeyRelease(Input::Key key, int mods) {
    switch (key) {
        case Input::Key::kEscape:
            if (CursorMode() == Input::Cursor::Mode::kDisabled) {
                SetCursorMode(Input::Cursor::Mode::kNormal);
            } else {
                p_vk_window_->Close();
            }
            break;
        default: BaseClass::OnKeyRelease(key, mods);
    }
}

void Phyre::Graphics::DrawCube::OnMousePress(Input::Mouse mouse_button, int mods) {
    if (CursorMode() == Input::Cursor::Mode::kNormal && (mouse_button == Input::Mouse::kButton1 || mouse_button == Input::Mouse::kLeftButton)) {
        SetCursorMode(Input::Cursor::Mode::kDisabled);
    }
    BaseClass::OnMousePress(mouse_button, mods);
}

void Phyre::Graphics::DrawCube::StartDebugger() {
    debugger_.InitializeDebugReport();
}

void Phyre::Graphics::DrawCube::LoadGPUs() {
    gpus_.clear();
    std::vector<vk::PhysicalDevice> physical_device_vector = instance_.get().enumeratePhysicalDevices();
    for (const vk::PhysicalDevice& physical_device : physical_device_vector) {
        const vk::PhysicalDeviceProperties properties = physical_device.getProperties();
        PHYRE_LOG(info, kWho) << "Found device: " << properties.deviceName;
        gpus_.emplace_back(VulkanGPU(physical_device));
    }
    if (gpus_.empty()) {
        std::string error_message = "Could not locate any GPUs";
        PHYRE_LOG(fatal, kWho) << error_message;
        throw std::runtime_error(error_message);
    }
    p_active_gpu_ = &gpus_[0];
}

void Phyre::Graphics::DrawCube::LoadWindow(float width, float height, const std::string& title) {
    p_vk_window_ = VulkanWindow::Create(width, height, title, instance_, *p_active_gpu_);
}

void Phyre::Graphics::DrawCube::LoadDevice() {
    p_device_ = new VulkanDevice(*p_active_gpu_, *p_vk_window_);
}

void Phyre::Graphics::DrawCube::LoadCommandPool() {
    /**
    * Command buffers reside in command buffer pools.
    * This is necessary for allocating command buffers
    * because memory is coarsly allocated in large chunks between the CPU and GPU.
    */
    vk::CommandPoolCreateInfo command_pool_create_info(vk::CommandPoolCreateFlags(), p_device_->graphics_queue_family_index());

    // The command buffers allocated from this pool will be freed after a relatively short period of time.
    // We may also reuse these command buffers multiple times
    command_pool_create_info.setFlags(vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    command_pool_ = p_device_->get().createCommandPool(command_pool_create_info);
}

void Phyre::Graphics::DrawCube::LoadCommandBuffers() {
    // We need to reload the command buffers when the swapchain gets reloaded
    if (!command_buffers_.empty()) {
        p_device_->get().freeCommandBuffers(command_pool_, static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());
    }
    vk::CommandBufferAllocateInfo command_buffer_allocate_info;
    uint32_t command_buffer_count = 1;
    command_buffer_allocate_info.setCommandPool(command_pool_);
    command_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);
    command_buffer_allocate_info.setCommandBufferCount(command_buffer_count);
    
    command_buffers_ = p_device_->get().allocateCommandBuffers(command_buffer_allocate_info);
}

void Phyre::Graphics::DrawCube::ExecuteBeginCommandBuffer(size_t command_buffer_index) {
    if (command_buffer_index > command_buffers_.size()) {
        PHYRE_LOG(error, kWho) << "Failed to begin command buffer: index out of bounds";
        return;
    }

    vk::CommandBufferBeginInfo info;
    info.setPInheritanceInfo(nullptr);
    command_buffers_[command_buffer_index].begin(&info);
}

void Phyre::Graphics::DrawCube::LoadSwapchain() {
    p_swapchain_.reset(new VulkanSwapchain(*p_device_, *p_vk_window_, p_swapchain_.get()));
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
    vertex_input_state_create_info.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertex_input_attributes_.size()));
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
    
    color_blend_state_create_info.setAttachmentCount(static_cast<uint32_t>(color_blend_attachment_states.size()));
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
    dynamic_state_create_info.setDynamicStateCount(static_cast<uint32_t>(dynamic_states.size()));
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
    graphics_pipeline_create_info.setStageCount(static_cast<uint32_t>(shader_stages_.size()));
    graphics_pipeline_create_info.setSubpass(0);
    graphics_pipeline_create_info.setRenderPass(p_render_pass_->get());
    
    pipeline_ = p_device_->get().createGraphicsPipeline(pipeline_cache_, graphics_pipeline_create_info);
}

void Phyre::Graphics::DrawCube::LoadSemaphores() {
    vk::SemaphoreCreateInfo info;
    render_finished_semaphore_ = p_device_->get().createSemaphore(info);
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
    vk::RenderPassBeginInfo render_pass_begin_info;
    render_pass_begin_info.setRenderPass(p_render_pass_->get());
    render_pass_begin_info.setFramebuffer(framebuffers_[p_swapchain_->current_frame_index()]);

    vk::Offset2D offset;
    vk::Extent2D extent(static_cast<uint32_t>(p_swapchain_->image_width()), static_cast<uint32_t>(p_swapchain_->image_height()));
    vk::Rect2D render_area(offset, extent);
    render_pass_begin_info.setRenderArea(render_area);

    render_pass_begin_info.setClearValueCount(static_cast<uint32_t>(clear_values.size()));
    render_pass_begin_info.setPClearValues(clear_values.data());

    // We can only connect vertex buffers between the begin and end stages of a render pass
    // Begin the render pass stage
    uint32_t command_buffer_index = 0;
    const vk::CommandBuffer& command_buffer = command_buffers_[command_buffer_index];
    ExecuteBeginCommandBuffer(command_buffer_index);
    command_buffer.beginRenderPass(&render_pass_begin_info, vk::SubpassContents::eInline);

    // Bind vertex buffers
    uint32_t start_binding = 0; // We will start our binding at offset 0
    uint32_t binding_count = 1; // We only have one vertex buffer
    std::array<vk::DeviceSize, 1> offsets = { 0 };
    command_buffer.bindVertexBuffers(start_binding, binding_count, &vertex_buffer_.buffer, offsets.data());

    // Bind graphics pipeline to the command buffer
    // Note that it is also possible to create multiple graphics pipelines and switch between them with a single
    // command buffer
    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_);

    // Bind the descriptor sets to the command buffer
    // Now the pipeline will know how to find its input data like the MVP transform
    uint32_t first_set = 0;
    command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout_, first_set, 1, descriptor_sets_.data(), 0, nullptr);

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
    scissor_extent.setWidth(static_cast<uint32_t>(p_swapchain_->image_width()));
    scissor_extent.setHeight(static_cast<uint32_t>(p_swapchain_->image_height()));

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
    submit_info.setPWaitSemaphores(&p_swapchain_->image_acquired_semaphore()); // This forces a wait until the image is ready before drawing so that we know when the swapchain image is available
                                                                // When the GPU is done executing commands, it signals the fence to indicate that the drawing is complete
    submit_info.setPWaitDstStageMask(&pipeline_stage_flags);
    submit_info.setCommandBufferCount(static_cast<uint32_t>(command_buffers.size()));
    submit_info.setPCommandBuffers(command_buffers.data());
    submit_info.setSignalSemaphoreCount(1);
    submit_info.setPSignalSemaphores(&render_finished_semaphore_);

    // Submit to the queue
    p_device_->graphics_queue().submit(1, &submit_info, nullptr);
}

void Phyre::Graphics::DrawCube::BeginRender() const {
    p_swapchain_->AcquireNextImage();
    p_device_->graphics_queue().waitIdle();
}

void Phyre::Graphics::DrawCube::EndRender() {
    // Now present the image to the window
    vk::PresentInfoKHR present_info;
    present_info.setSwapchainCount(1);
    present_info.setPSwapchains(&p_swapchain_->swapchain());
    present_info.setPImageIndices(&p_swapchain_->current_frame_index());
    present_info.setWaitSemaphoreCount(1);
    present_info.setPWaitSemaphores(&render_finished_semaphore_);
    present_info.setPResults(nullptr);

    // We always want the swapchain to be in an optimal state
    vk::Result present_result = p_device_->presentation_queue().presentKHR(&present_info);
    if (present_result == vk::Result::eErrorOutOfDateKHR || present_result == vk::Result::eSuboptimalKHR) {
        ReloadSwapchain();
    }
}

void Phyre::Graphics::DrawCube::Stop() const {
    p_device_->get().waitIdle();
}

void Phyre::Graphics::DrawCube::LogFPS() {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::seconds;
    
    static uint32_t frames = 0;
    ++frames;
    static high_resolution_clock::time_point start = high_resolution_clock::now();
    high_resolution_clock::time_point now = high_resolution_clock::now();
    if (now - start > std::chrono::milliseconds(1000)) {
        PHYRE_LOG(info, kWho) << "FPS: " << frames;
        start = now;
        frames = 0;
    }   
}

void Phyre::Graphics::DrawCube::LoadShaderModules() {
    // This is where the SPIR-V intermediate bytecode is located
    std::string vertex_file_name("vertices.spv");
    std::vector<uint32_t> vertex_shader_bytecode = p_provider_->GetContentsSPIRV(target_, vertex_file_name);

    if (vertex_shader_bytecode.empty()) {
        throw std::runtime_error("Could not read any vertex shader bytecode!");
    }

    std::string fragment_file_name("fragments.spv");
    std::vector<uint32_t> fragment_shader_bytecode = p_provider_->GetContentsSPIRV(target_, fragment_file_name);

    if (vertex_shader_bytecode.empty()) {
        throw std::runtime_error("Could not read any fragment shader bytecode!");
    }

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
    if (!VulkanUtils::CanFindMemoryTypeFromProperties(p_device_->gpu(), memory_requirements.memoryTypeBits, flags, allocate_info.memoryTypeIndex)) {
        PHYRE_LOG(fatal, kWho) << "Failed to get memory type from properties while allocating vertex buffer";
    }
    vertex_buffer_.memory = p_device_->get().allocateMemory(allocate_info);

    void *p_data = p_device_->get().mapMemory(vertex_buffer_.memory, 0, memory_requirements.size);
    memcpy(p_data, Geometry::kVertexBufferSolidFaceColorData.data(), data_size);
    p_device_->get().unmapMemory(vertex_buffer_.memory);
    p_device_->get().bindBufferMemory(vertex_buffer_.buffer, vertex_buffer_.memory, 0);

    uint32_t stride = sizeof(Geometry::Vertex);
    vertex_input_binding_description_.setBinding(0);
    vertex_input_binding_description_.setInputRate(vk::VertexInputRate::eVertex);
    vertex_input_binding_description_.setStride(stride);

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
    uint32_t graphics_queue_family_index = p_device_->graphics_queue_family_index();
    
    vk::BufferCreateInfo uniform_buffer_create_info;
    uniform_buffer_create_info.setPQueueFamilyIndices(&graphics_queue_family_index);
    uniform_buffer_create_info.setQueueFamilyIndexCount(1);
    uniform_buffer_create_info.setSharingMode(vk::SharingMode::eExclusive);
    uniform_buffer_create_info.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
    uniform_buffer_create_info.setSize(sizeof(ModelViewProjection));
    
    vk::Buffer buffer = p_device_->get().createBuffer(uniform_buffer_create_info);

    // The memory requirements come with size, alignment, and memory type
    // They vary accross GPU
    vk::MemoryRequirements memory_requirements = p_device_->get().getBufferMemoryRequirements(buffer);

    // We need the CPU to access this memory. The eHostCoherent bit is set because we want to write 
    // to this memory witout flushing (i.e. synchronizing after every update).
    vk::MemoryPropertyFlags memory_property_flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    // NOTE This may crash if no memory_type_index can be found
    uint32_t memory_type_index = p_active_gpu_->FindMemoryTypeIndex(memory_requirements.memoryTypeBits, memory_property_flags);
    vk::MemoryAllocateInfo malloc_info;
    malloc_info.setAllocationSize(memory_requirements.size);
    malloc_info.setMemoryTypeIndex(memory_type_index);

    // Allocate some memory on the device
    vk::DeviceMemory memory = p_device_->get().allocateMemory(malloc_info);

    // Bind the memory to the buffer
    p_device_->get().bindBufferMemory(buffer, memory, 0);

    // Let's map this memory to the host
    vk::DescriptorBufferInfo descriptor;
    descriptor.setBuffer(buffer);
    descriptor.setOffset(0);                          // Memory in this buffer is accessed realtive to this offset
    descriptor.setRange(sizeof(ModelViewProjection)); // The range in bytes starting from offset
    p_uniform_buffer_ = std::make_unique<UniformBuffer>(*p_device_, buffer, memory, descriptor);

    UpdateUniformBuffers();
}

void Phyre::Graphics::DrawCube::UpdateUniformBuffers() const {
    ModelViewProjection mvp;

    // TODO This is just a proof of concept for now, ideally, we would like to specify which model
    // to rotate, more parameters, etc...
    glm::vec2 cursor_position = CursorPosition();

    // Rotate 90 degrees per second along the X-Axis
    mvp.model = glm::mat4();
    mvp.model = glm::rotate(mvp.model, cursor_position.x * glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    mvp.model = glm::rotate(mvp.model, cursor_position.y * glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Set the "camera" eye position at the origin looking upwards
    glm::vec3 eye(4.0f, 0.0f, 0.0f);        // The position of our eye in the world
    glm::vec3 center(0.0f, 0.0f, 0.0f);     // Where we want to look at in the world
    glm::vec3 up(0.0f, 1.0f, 0.0f);         // Unit vector specifying which axis is UP in our world
    mvp.view = glm::lookAt(eye, center, up);

    // Project the 3D world onto a plane
    static const auto field_of_view = glm::radians(60.0f); // The camera lens: more degrees is wider, less is more zoomed in
    float aspect_ratio = p_swapchain_->image_width() / p_swapchain_->image_height(); // Framebuffer resolution (e.g 1920x1080)
    static const float near_clipping_plane = 0.1f;
    static const float far_clipping_plane = 100.0f;
    mvp.projection = glm::perspective(field_of_view, aspect_ratio, near_clipping_plane, far_clipping_plane);

    // Flip the Y-coordinate since GLM was originally designed for OpenGL
    mvp.projection[1][1] *= -1;

    // Map the memory from the device to the host, update it with the new transformation data, then remove the mapping
    void *p_data = p_device_->get().mapMemory(p_uniform_buffer_->memory(), 0, sizeof(ModelViewProjection));
    memcpy(p_data, &mvp, sizeof(ModelViewProjection));
    p_device_->get().unmapMemory(p_uniform_buffer_->memory());
}

void Phyre::Graphics::DrawCube::LoadFrameBuffers() {
    if (!framebuffers_.empty()) {
        DestroyFramebuffers();
    }

    uint32_t color = 0;
    uint32_t depth = 1;
    std::array<vk::ImageView, 2> attachments;
    attachments[depth] = p_swapchain_->depth_image().image_view;

    vk::FramebufferCreateInfo info;
    info.setRenderPass(p_render_pass_->get());
    info.setAttachmentCount(static_cast<uint32_t>(attachments.size())); // 2 Because we are using image and depth
    info.setPAttachments(attachments.data());
    info.setWidth(static_cast<uint32_t>(p_swapchain_->image_width()));
    info.setHeight(static_cast<uint32_t>(p_swapchain_->image_height()));
    info.setLayers(1);

    framebuffers_ = std::vector<vk::Framebuffer>(p_swapchain_->swapchain_images().size());
    for (uint32_t i = 0; i < p_swapchain_->swapchain_images().size(); ++i) {
        attachments[color] = p_swapchain_->swapchain_images()[i].image_view;
        framebuffers_[i] = p_device_->get().createFramebuffer(info);
    }
}

void Phyre::Graphics::DrawCube::LoadRenderPass() {
    p_render_pass_.reset(new VulkanRenderPass(*p_device_, *p_swapchain_));
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

    // Specify the descriptor set that we will write to
    // In GLSL, we can specify uniform buffers and associate them with sets (i.e. descriptor sets)
    std::array<vk::WriteDescriptorSet, 1> writes;
    writes[0] = vk::WriteDescriptorSet();
    writes[0].setDstSet(descriptor_sets_[0]);     // The destination descriptor set to update
    writes[0].setDescriptorCount(1);              // How many elements in the descriptor to update
    writes[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
    writes[0].setPBufferInfo(&p_uniform_buffer_->descriptor());
    writes[0].setDstArrayElement(0);              // The first element to update
    writes[0].setDstBinding(0);                   // The destination descriptor binding to update

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
        PHYRE_LOG(error, kWho) << "Failed to load pipeline layout: No logical device";
        return;
    }
    vk::PipelineLayoutCreateInfo create_info;
    create_info.setPushConstantRangeCount(0);
    create_info.setPPushConstantRanges(nullptr);
    create_info.setSetLayoutCount(1);
    create_info.setPSetLayouts(&descriptor_set_layout_);

    pipeline_layout_ = p_device_->get().createPipelineLayout(create_info);
}