#include "vulkan_render_pass.h"
#include "logging.h"
#include "vulkan_errors.h"
#include "vulkan_swapchain.h"
#include "geometry.h"
#include <fstream>
#include "vulkan_pipeline.h"

const std::string Phyre::Graphics::VulkanRenderPass::kWho = "[VulkanRenderPass]";

Phyre::Graphics::VulkanRenderPass::VulkanRenderPass(const vk::Device& device, const VulkanSwapchain& swapchain_manager, const VulkanMemoryManager& memory_manager) :
    device_(device),
    swapchain_manager_(swapchain_manager),
    render_pass_(InitializeRenderPass(device_, swapchain_manager.samples(), swapchain_manager.image_format(), swapchain_manager.depth_format())),
    pipeline_shader_stages_(InitializeShaderStages(device_)),
    shader_modules_(InitializeShaderModules(pipeline_shader_stages_)),
    framebuffers_ (InitializeFramebuffers(device_,
                                          swapchain_manager.depth_image().image_view,
                                          render_pass_,
                                          swapchain_manager.image_width(),
                                          swapchain_manager.image_height(),
                                          swapchain_manager.swapchain_images())),
    vertex_buffer_(InitializeVertexBuffer(device, memory_manager.gpu())),
    vertex_input_binding_description_(InitializeVertexInputBindingDescription()),
    vertex_input_attribute_descriptions_(InitializeVertexInputAttributeDescriptions()),
    image_acquired_semaphore_(InitializeImageAcquiredSemaphore(device_)),
    current_swapchain_image_index_(InitializeCurrentSwapchainImageIndex(device, swapchain_manager.swapchain(), image_acquired_semaphore_)){
    Logging::trace("Initialized", kWho);
}

Phyre::Graphics::VulkanRenderPass::~VulkanRenderPass() {
    device_.destroySemaphore(image_acquired_semaphore_);
    device_.freeMemory(vertex_buffer_.device_memory);
    device_.destroyBuffer(vertex_buffer_.buffer);
    for (const vk::Framebuffer& framebuffer : framebuffers_) {
        device_.destroyFramebuffer(framebuffer);
    }
    for (const vk::ShaderModule& shader_module : shader_modules_) {
        device_.destroyShaderModule(shader_module);
    }
    device_.destroyRenderPass(render_pass_);
    Logging::trace("Destroyed", kWho);
}

vk::RenderPass Phyre::Graphics::VulkanRenderPass::InitializeRenderPass(const vk::Device& device,
                                                                       vk::SampleCountFlagBits num_samples,
                                                                       vk::Format image_format,
                                                                       vk::Format depth_format) {
    // We have two attachments for our render pass: one for color, and one for depth
    std::array<vk::AttachmentDescription, 2> attachments;

    // Color attachment
    attachments[0].setFormat(image_format);
    attachments[0].setSamples(num_samples);
    attachments[0].setLoadOp(vk::AttachmentLoadOp::eClear); // We want the buffer to be cleared at the start of the render pass instance
    attachments[0].setStoreOp(vk::AttachmentStoreOp::eStore); // We want to leave the rendering result in this buffer so that it can be presented to the display
    attachments[0].setStencilLoadOp(vk::AttachmentLoadOp::eDontCare); // We don't need the contents of the buffer when the render pass instance is complete.
    attachments[0].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    attachments[0].setInitialLayout(vk::ImageLayout::eUndefined); // This is where the attachment image subresource will be in when a render pass instance begins
                                                                  // Starting as undefined means the GPU has to do less work while running its image layout algorithms
    attachments[0].setFinalLayout(vk::ImageLayout::ePresentSrcKHR); // This is where the attachment image subresource will be in when a render pass instance ends
                                                                    // It is the optimal layout for the display hardware to read the rendered GPU images

    // Depth attachment
    attachments[1].setFormat(depth_format);
    attachments[1].setSamples(num_samples);
    attachments[1].setLoadOp(vk::AttachmentLoadOp::eClear);
    attachments[1].setStoreOp(vk::AttachmentStoreOp::eDontCare);
    attachments[1].setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    attachments[1].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    attachments[1].setInitialLayout(vk::ImageLayout::eUndefined);
    attachments[1].setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    // Initialize subpasses to to indicate which attachments are active during the subpass
    // We can also specify the layouts to be used while rendering during the subpass
    vk::AttachmentReference color_reference;
    color_reference.setAttachment(0);  // These indices refer to the array indicies of the previous attachments
    color_reference.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
    
    vk::AttachmentReference depth_reference;
    depth_reference.setAttachment(1);
    depth_reference.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass_description;
    subpass_description.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass_description.setInputAttachmentCount(0);
    subpass_description.setPInputAttachments(nullptr);
    subpass_description.setColorAttachmentCount(1);
    subpass_description.setPColorAttachments(&color_reference);
    subpass_description.setPResolveAttachments(nullptr);
    subpass_description.setPDepthStencilAttachment(&depth_reference);
    subpass_description.setPreserveAttachmentCount(0);
    subpass_description.setPResolveAttachments(nullptr);

    // Create the render pass
    vk::RenderPassCreateInfo create_info;
    create_info.setAttachmentCount(2);
    create_info.setPAttachments(attachments.data());
    create_info.setSubpassCount(1);
    create_info.setPSubpasses(&subpass_description);
    create_info.setDependencyCount(0);
    create_info.setPDependencies(nullptr);

    vk::RenderPass render_pass;
    vk::Result result = device.createRenderPass(&create_info, nullptr, &render_pass);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to create render pass", kWho);
    }
    return render_pass;
}

Phyre::Graphics::VulkanRenderPass::PipelineShaderStages Phyre::Graphics::VulkanRenderPass::InitializeShaderStages(const vk::Device& device) {
    // This is where the SPIR-V intermediate bytecode is located
    std::string resource_directory("GraphicsTestResources/");
    std::string vertex_file_name(resource_directory + "vertices.spv");
    std::vector<uint32_t> vertex_shader_bytecode(ReadSpirV(vertex_file_name));

    std::string fragment_file_name(resource_directory + "fragments.spv");
    std::vector<uint32_t> fragment_shader_bytecode(ReadSpirV(fragment_file_name));

    // We will be creating two pipeline shader stages: One for vertices, and one for fragments
    // Create the vertex shader module
    uint32_t shader_module_count = 2;
    PipelineShaderStages shader_stage_create_infos(shader_module_count, vk::PipelineShaderStageCreateInfo());
    shader_stage_create_infos[0].setPSpecializationInfo(nullptr);
    shader_stage_create_infos[0].setStage(vk::ShaderStageFlagBits::eVertex);
    shader_stage_create_infos[0].setPName("main");

    vk::ShaderModuleCreateInfo vertex_shader_module_create_info;
    vertex_shader_module_create_info.setCodeSize(vertex_shader_bytecode.size() * sizeof(uint32_t));
    vertex_shader_module_create_info.setPCode(vertex_shader_bytecode.data());

    vk::Result result = device.createShaderModule(&vertex_shader_module_create_info, nullptr, &shader_stage_create_infos[0].module);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to create vertex shader module!", kWho);
    }

    // Create the fragment shader module
    shader_stage_create_infos[1].setPSpecializationInfo(nullptr);
    shader_stage_create_infos[1].setStage(vk::ShaderStageFlagBits::eFragment);
    shader_stage_create_infos[1].setPName("main");

    vk::ShaderModuleCreateInfo fragment_shader_module_create_info;
    fragment_shader_module_create_info.setCodeSize(fragment_shader_bytecode.size() * sizeof(uint32_t));
    fragment_shader_module_create_info.setPCode(fragment_shader_bytecode.data());

    result = device.createShaderModule(&fragment_shader_module_create_info, nullptr, &shader_stage_create_infos[1].module);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to create fragment shader module!", kWho);
    }
    return shader_stage_create_infos;
}

Phyre::Graphics::VulkanRenderPass::ShaderModuleVector Phyre::Graphics::VulkanRenderPass::InitializeShaderModules(const PipelineShaderStages& shader_stages) {
    ShaderModuleVector shader_modules(shader_stages.size());
    std::transform(shader_stages.begin(), shader_stages.end(), shader_modules.begin(), [](const vk::PipelineShaderStageCreateInfo& info)
    {
        return info.module;
    });
    return shader_modules;
}

Phyre::Graphics::VulkanRenderPass::FramebufferVector Phyre::Graphics::VulkanRenderPass::InitializeFramebuffers(const vk::Device& device,
                                                                                                               const vk::ImageView& depth_image_view,
                                                                                                               const vk::RenderPass& render_pass,
                                                                                                               const uint32_t width,
                                                                                                               const uint32_t height,
                                                                                                               const VulkanSwapchain::SwapchainImageVector& swapchain_imges) {
    uint32_t color_index = 0;
    uint32_t depth_index = 1;
    std::array<vk::ImageView, 2> attachments;
    attachments[depth_index] = depth_image_view;

    vk::FramebufferCreateInfo create_info;
    create_info.setRenderPass(render_pass);
    create_info.setAttachmentCount(attachments.size());
    create_info.setPAttachments(attachments.data());
    create_info.setWidth(width);
    create_info.setHeight(height);
    create_info.setLayers(1);

    // The number of swapchain buffers are determined at runtime
    FramebufferVector framebuffers(swapchain_imges.size());
    for (uint32_t i = 0; i < swapchain_imges.size(); ++i) {
        // We set the color image buffers into the swapchain since the depth image view will remain the same
        attachments[color_index] = swapchain_imges[i].image_view;
        vk::Result result = device.createFramebuffer(&create_info, nullptr, &framebuffers[i]);
        if (!ErrorCheck(result, kWho)) {
            Logging::fatal("Failed to create framebuffer", kWho);
        }
    }
    return framebuffers;
}

Phyre::Graphics::VulkanRenderPass::VertexBuffer
Phyre::Graphics::VulkanRenderPass::InitializeVertexBuffer(const vk::Device& device, const VulkanGPU& gpu) {
    vk::BufferCreateInfo create_info;
    create_info.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
    create_info.setSize(sizeof(Geometry::kVertexBufferSolidFaceColorData));
    create_info.setQueueFamilyIndexCount(0);
    create_info.setPQueueFamilyIndices(nullptr);
    create_info.setSharingMode(vk::SharingMode::eExclusive);
    
    vk::Buffer buffer;
    vk::Result result = device.createBuffer(&create_info, nullptr, &buffer);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to create vertex buffer", kWho);
    }

    // Follow the same procedure for allocating memory just like with a uniform buffer
    vk::MemoryRequirements memory_requirements;
    device.getBufferMemoryRequirements(buffer, &memory_requirements);

    vk::MemoryAllocateInfo memory_allocate_info;
    memory_allocate_info.setAllocationSize(memory_requirements.size);
    vk::MemoryPropertyFlags requirements_mask(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    if (!VulkanMemoryManager::CanFindMemoryTypeFromProperties(gpu, memory_requirements.memoryTypeBits, requirements_mask, memory_allocate_info.memoryTypeIndex)) {
        Logging::fatal("Failed to satisfy memory allocation requirements for creating vertex buffer. The memory could not be mapped.", kWho);
    }

    vk::DeviceMemory device_memory;
    result = device.allocateMemory(&memory_allocate_info, nullptr, &device_memory);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to allocate device memory for creating vertex buffer", kWho);
    }

    // Temporarily give the CPU access to this memory
    vk::MemoryMapFlags flags;
    void* p_data;
    result = device.mapMemory(device_memory, 0, memory_requirements.size, flags, static_cast<void**>(&p_data));
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to map data to memory on physical device for creating vertex buffer", kWho);
    }

    memcpy(p_data, Geometry::kVertexBufferSolidFaceColorData, sizeof(Geometry::kVertexBufferSolidFaceColorData));

    // Immediately unmap the memory because the page tables are limited in size for memory visible to
    // both CPU and GPU
    device.unmapMemory(device_memory);
    device.bindBufferMemory(buffer, device_memory, 0);

    VertexBuffer vertex_buffer;
    vertex_buffer.buffer = buffer;
    vertex_buffer.device_memory = device_memory;
    return vertex_buffer;
}

vk::VertexInputBindingDescription Phyre::Graphics::VulkanRenderPass::InitializeVertexInputBindingDescription() {
    vk::VertexInputBindingDescription binding_description;
    binding_description.setBinding(0); // Refers to its respective values in the GLSL shader code
    binding_description.setInputRate(vk::VertexInputRate::eVertex);
    binding_description.setStride(sizeof(Geometry::kVertexBufferSolidFaceColorData)); // The size needed to add to a pointer in order to reach the next vertex
    return binding_description;
}

Phyre::Graphics::VulkanRenderPass::VertexInputAttributeDescriptions Phyre::Graphics::VulkanRenderPass::InitializeVertexInputAttributeDescriptions() {
    VertexInputAttributeDescriptions attribute_descriptions;
    attribute_descriptions[0].setBinding(0); // Refers to its respective values in the GLSL shader code
    attribute_descriptions[0].setLocation(0); // Refers to its respective values in the GLSL shader code
    attribute_descriptions[0].setFormat(vk::Format::eR32G32B32A32Sfloat);
    attribute_descriptions[0].setOffset(0);
    attribute_descriptions[1].setBinding(0);
    attribute_descriptions[1].setLocation(1);
    attribute_descriptions[1].setFormat(vk::Format::eR32G32B32A32Sfloat);
    attribute_descriptions[1].setOffset(16);
    return attribute_descriptions;
}

uint32_t Phyre::Graphics::VulkanRenderPass::InitializeCurrentSwapchainImageIndex(const vk::Device& device, const vk::SwapchainKHR& swapchain, const vk::Semaphore& image_acquired_semaphore) {
    // Get the index of the next available swapchain image
    vk::ResultValue<uint32_t> current_swapchain_image_index = device.acquireNextImageKHR(swapchain, UINT64_MAX, image_acquired_semaphore, nullptr);
    return current_swapchain_image_index.value;
}

vk::Semaphore Phyre::Graphics::VulkanRenderPass::InitializeImageAcquiredSemaphore(const vk::Device& device) {
    // We need to get an image from the swapchain in order to draw anything
    // Semaphores are used to ensure that all images in the swapchain are currently available
    vk::SemaphoreCreateInfo semaphore_create_info;
    vk::Semaphore image_acquired_semaphore = device.createSemaphore(semaphore_create_info);
    return image_acquired_semaphore;
}

std::vector<uint32_t> Phyre::Graphics::VulkanRenderPass::ReadSpirV(const std::string spirv_shader_file_name) {
    std::ifstream shader_file(spirv_shader_file_name, std::ios::binary);
    if (!shader_file) {
        std::ostringstream oss;
        oss << "Could not open " << spirv_shader_file_name;
        Logging::error(oss.str(), kWho);
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

void Phyre::Graphics::VulkanRenderPass::ExecuteBeginCommandBuffer(const vk::CommandBuffer& command_buffer) {
    vk::CommandBufferBeginInfo info;
    command_buffer.begin(info);
}

// TODO: Name this as Begin (member method) and call it in the pipeline's constructor
// This is because we are going to need to initialize a command buffer which should bind everything to the pipeline
void Phyre::Graphics::VulkanRenderPass::BeginRenderPass(const vk::CommandBuffer& command_buffer,
                                                        const VulkanPipeline& pipeline,
                                                        const vk::Queue& graphics_queue,
                                                        const vk::Queue& presentation_queue) const {
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
    begin_info.setRenderPass(render_pass_);
    begin_info.setFramebuffer(framebuffers_[current_swapchain_image_index_]);
    
    vk::Offset2D offset;
    vk::Extent2D extent(swapchain_manager_.image_width(), swapchain_manager_.image_height());
    vk::Rect2D render_area(offset, extent);
    begin_info.setRenderArea(render_area);

    begin_info.setClearValueCount(clear_values.size());
    begin_info.setPClearValues(clear_values.data());

    // We can only connect vertex buffers between the begin and end stages of a render pass
    // Begin the render pass stage
    ExecuteBeginCommandBuffer(command_buffer);
    command_buffer.beginRenderPass(&begin_info, vk::SubpassContents::eInline);
    
    // Bind vertex buffers
    uint32_t start_binding = 0; // We will start our binding at offset 0
    uint32_t binding_count = 1; // We only have one vertex buffer
    std::array<vk::DeviceSize, 1> offsets = {0};
    command_buffer.bindVertexBuffers(start_binding, binding_count, &vertex_buffer_.buffer, offsets.data());

    // Bind graphics pipeline
    // Note that it is also possible to create multiple graphics pipelines and switch between them with a single
    // command buffer
    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline());

    // Bind the descriptor sets
    // Now the pipeline will know how to find its input data like the MVP transform
    uint32_t first_set = 0;
    command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.pipeline_layout(), first_set, 1, &pipeline.descriptor_set(), 0, nullptr);
    // command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.pipeline_layout(), first_set, pipeline.descriptor_set(), nullptr);

    // We bind the viewports and scissors dynamically because earlier we specified that they would be dynamically
    // loaded from a command buffer.
    // A reason to keep these dynamic is becuase most applications need to update these values if the window size
    // changes during execution. This avoids having to rebuild the pipeline when the window size changes.
    // Bind the viewports
    vk::Viewport viewport;
    viewport.setHeight(swapchain_manager_.image_height());
    viewport.setWidth(swapchain_manager_.image_width());
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);
    viewport.setX(0);
    viewport.setY(0);
    
    uint32_t first_viewport = 0;
    command_buffer.setViewport(first_viewport, viewport);

    // Bind the scissors
    vk::Rect2D scissor;
    vk::Extent2D scissor_extent;
    scissor_extent.setWidth(swapchain_manager_.image_width());
    scissor_extent.setHeight(swapchain_manager_.image_height());

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

    // Create a fence which we can use to know when the GPU is done rendering
    // This is so that we can be guaranteed to display fully rendered images
    vk::FenceCreateInfo fence_create_info;
    vk::Fence fence = device_.createFence(fence_create_info);

    // Finally, submit the command buffer
    std::array<vk::CommandBuffer, 1> command_buffers { command_buffer };
    vk::PipelineStageFlags pipeline_stage_flags(vk::PipelineStageFlagBits::eBottomOfPipe); // The final stage in the pipeline where the commands finish execution
    
    vk::SubmitInfo submit_info;
    submit_info.setWaitSemaphoreCount(1);
    submit_info.setPWaitSemaphores(&image_acquired_semaphore_); // This forces a wait until the image is ready before drawing so that we know when the swapchain image is available
                                                                // When the GPU is done executing commands, it signals the fence to indicate that the drawing is complete
    submit_info.setPWaitDstStageMask(&pipeline_stage_flags);
    submit_info.setCommandBufferCount(command_buffers.size());
    submit_info.setPCommandBuffers(command_buffers.data());
    submit_info.setSignalSemaphoreCount(0);
    submit_info.setPSignalSemaphores(nullptr);

    // Submit to the queue
    graphics_queue.submit(1, &submit_info, fence);

    // Now present the image to the window
    vk::PresentInfoKHR present_info;
    present_info.setSwapchainCount(1);
    present_info.setPSwapchains(&swapchain_manager_.swapchain());
    present_info.setPImageIndices(&current_swapchain_image_index_);
    present_info.setWaitSemaphoreCount(0);
    present_info.setPWaitSemaphores(nullptr);
    present_info.setPResults(nullptr);

    // Make sure the command buffer finished before presenting
    vk::Result result = vk::Result::eTimeout;
    do {
        bool wait_all = true;
        uint64_t timeout = 100000000; // The amount of time in nanoseconds for a command buffer to complete
        result = device_.waitForFences(1, &fence, wait_all, timeout);
    } while (result == vk::Result::eTimeout);


    presentation_queue.presentKHR(&present_info);
    Sleep(1000);
}
