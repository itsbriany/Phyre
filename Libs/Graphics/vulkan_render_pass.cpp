#include "vulkan_render_pass.h"
#include "logging.h"
#include "vulkan_errors.h"
#include "swapchain_manager.h"

const std::string Phyre::Graphics::VulkanRenderPass::kWho = "[VulkanRenderPass]";

Phyre::Graphics::VulkanRenderPass::VulkanRenderPass(const vk::Device& device, const SwapchainManager& swapchain_manager) :
    device_(device),
    render_pass_(InitializeRenderPass(device_, swapchain_manager.samples(), swapchain_manager.image_format(), swapchain_manager.depth_format())) {
    Logging::trace("Initialized", kWho);
}

Phyre::Graphics::VulkanRenderPass::~VulkanRenderPass() {
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
