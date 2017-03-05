#include "vulkan_render_pass.h"
#include "logging.h"
#include "vulkan_window.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"


const std::string Phyre::Graphics::VulkanRenderPass::kWho = "[VulkanRenderPass]";

Phyre::Graphics::VulkanRenderPass::VulkanRenderPass(const VulkanDevice& device, const VulkanSwapchain& swapchain) :
    device_(device),
    swapchain_(swapchain),
    render_pass_(LoadRenderPass(device_, swapchain_)),
    framebuffers_(LoadFramebuffers(device_, swapchain_, render_pass_)),
    image_acquired_semaphore_(LoadImageAcquiredSemaphore(device)),
    swapchain_current_index_(LoadSwapchainCurrentIndex(device_.get(), swapchain_.swapchain(), image_acquired_semaphore_))
{
    Logging::trace("Initialized", kWho);
}

Phyre::Graphics::VulkanRenderPass::~VulkanRenderPass() {
    device_.get().destroySemaphore(image_acquired_semaphore_);
    DestroyFramebuffers();
    device_.get().destroyRenderPass(render_pass_);
    Logging::trace("Destroyed", kWho);
}

vk::RenderPass Phyre::Graphics::VulkanRenderPass::LoadRenderPass(const VulkanDevice& device, const VulkanSwapchain& swapchain) {
    uint32_t color = 0;
    uint32_t depth = 1;
    std::array<vk::AttachmentDescription, 2> attachments;
    attachments[color].setFormat(swapchain.window().preferred_surface_format().format);
    attachments[color].setSamples(swapchain.samples());
    attachments[color].setLoadOp(vk::AttachmentLoadOp::eClear);
    attachments[color].setStoreOp(vk::AttachmentStoreOp::eStore);
    attachments[color].setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    attachments[color].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    attachments[color].setInitialLayout(vk::ImageLayout::eUndefined);
    attachments[color].setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

    attachments[depth].setFormat(swapchain.depth_format());
    attachments[depth].setSamples(swapchain.samples());
    attachments[depth].setLoadOp(vk::AttachmentLoadOp::eClear);
    attachments[depth].setStoreOp(vk::AttachmentStoreOp::eDontCare);
    attachments[depth].setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    attachments[depth].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    attachments[depth].setInitialLayout(vk::ImageLayout::eUndefined);
    attachments[depth].setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentReference color_reference;
    color_reference.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
    color_reference.setAttachment(color);

    vk::AttachmentReference depth_reference;
    depth_reference.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    depth_reference.setAttachment(depth);

    vk::SubpassDescription subpass;
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setInputAttachmentCount(0);
    subpass.setPInputAttachments(nullptr);
    subpass.setColorAttachmentCount(1);
    subpass.setPColorAttachments(&color_reference);
    subpass.setPResolveAttachments(nullptr);
    subpass.setPDepthStencilAttachment(&depth_reference);
    subpass.setPreserveAttachmentCount(0);
    subpass.setPPreserveAttachments(nullptr);

    vk::RenderPassCreateInfo info;
    info.setAttachmentCount(attachments.size());
    info.setPAttachments(attachments.data());
    info.setSubpassCount(1);
    info.setPSubpasses(&subpass);
    info.setDependencyCount(0);
    info.setPDependencies(nullptr);

    return device.get().createRenderPass(info);
}

std::vector<vk::Framebuffer> Phyre::Graphics::VulkanRenderPass::LoadFramebuffers(const VulkanDevice& device, 
                                                                                 const VulkanSwapchain& swapchain,
                                                                                 const vk::RenderPass& render_pass) {
    uint32_t color = 0;
    uint32_t depth = 1;
    std::array<vk::ImageView, 2> attachments;
    attachments[depth] = swapchain.depth_image().image_view;

    vk::FramebufferCreateInfo info;
    info.setRenderPass(render_pass);
    info.setAttachmentCount(attachments.size()); // 2 Because we are using image and depth
    info.setPAttachments(attachments.data());
    info.setWidth(swapchain.image_width());
    info.setHeight(swapchain.image_height());
    info.setLayers(1);

    std::vector<vk::Framebuffer> framebuffers(swapchain.swapchain_images().size());
    for (uint32_t i = 0; i < swapchain.swapchain_images().size(); ++i) {
        attachments[color] = swapchain.swapchain_images()[i].image_view;
        framebuffers[i] = device.get().createFramebuffer(info);
    }
    return framebuffers;
}

vk::Semaphore Phyre::Graphics::VulkanRenderPass::LoadImageAcquiredSemaphore(const VulkanDevice& device) {
    vk::SemaphoreCreateInfo semaphore_create_info;
    return device.get().createSemaphore(semaphore_create_info);
}

uint32_t Phyre::Graphics::VulkanRenderPass::LoadSwapchainCurrentIndex(const vk::Device& device,
                                                                      const vk::SwapchainKHR& swapchain,
                                                                      const vk::Semaphore& image_acquired_semaphore) {
    vk::ResultValue<uint32_t> current_swapchain_image_index = device.acquireNextImageKHR(swapchain, UINT64_MAX, image_acquired_semaphore, nullptr);
    return current_swapchain_image_index.value;
}

void Phyre::Graphics::VulkanRenderPass::DestroyFramebuffers() {
    for (const auto& framebuffer : framebuffers_) {
        device_.get().destroyFramebuffer(framebuffer);
    }
}
