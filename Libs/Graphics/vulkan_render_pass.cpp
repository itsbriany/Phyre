#include "vulkan_render_pass.h"
#include "logging.h"
#include "vulkan_errors.h"
#include "swapchain_manager.h"
#include <fstream>


const std::string Phyre::Graphics::VulkanRenderPass::kWho = "[VulkanRenderPass]";

Phyre::Graphics::VulkanRenderPass::VulkanRenderPass(const vk::Device& device, const SwapchainManager& swapchain_manager) :
    device_(device),
    render_pass_(InitializeRenderPass(device_, swapchain_manager.samples(), swapchain_manager.image_format(), swapchain_manager.depth_format())),
    shader_modules_(InitializeShaderModules(device_)) {
    Logging::trace("Initialized", kWho);
}

Phyre::Graphics::VulkanRenderPass::~VulkanRenderPass() {
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

Phyre::Graphics::VulkanRenderPass::ShaderModuleVector Phyre::Graphics::VulkanRenderPass::InitializeShaderModules(const vk::Device& device) {
    // This is where the SPIR-V intermediate bytecode is located
    std::string resource_directory("GraphicsTestResources/");
    std::string vertex_file_name(resource_directory + "vertices.spv");
    std::vector<uint32_t> vertex_shader_bytecode(ReadSpirV(vertex_file_name));

    std::string fragment_file_name(resource_directory + "fragments.spv");
    std::vector<uint32_t> fragment_shader_bytecode(ReadSpirV(fragment_file_name));

    // We will be creating two pipeline shader stages: One for vertices, and one for fragments
    // Create the vertex shader module
    uint32_t shader_module_count = 2;
    std::vector<vk::PipelineShaderStageCreateInfo> shader_stage_create_infos(shader_module_count, vk::PipelineShaderStageCreateInfo());
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

    ShaderModuleVector shader_modules(shader_stage_create_infos.size());
    std::transform(shader_stage_create_infos.begin(), shader_stage_create_infos.end(), shader_modules.begin(), [](const vk::PipelineShaderStageCreateInfo& info)
    {
        return info.module;
    });
    return shader_modules;
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
