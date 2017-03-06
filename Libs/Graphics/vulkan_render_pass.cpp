#include "vulkan_render_pass.h"
#include "logging.h"
#include "vulkan_window.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"


const std::string Phyre::Graphics::VulkanRenderPass::kWho = "[VulkanRenderPass]";

Phyre::Graphics::VulkanRenderPass::VulkanRenderPass(const VulkanDevice& device, const VulkanSwapchain& swapchain) :
    device_(device),
    swapchain_(swapchain),
    render_pass_(LoadRenderPass(device_, swapchain_))
{
    Logging::trace("Initialized", kWho);
}

Phyre::Graphics::VulkanRenderPass::~VulkanRenderPass() {
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
    attachments[color].setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

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