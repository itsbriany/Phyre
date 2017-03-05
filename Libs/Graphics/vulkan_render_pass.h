#pragma once

#include <vulkan.hpp>


namespace Phyre {
namespace Graphics {

class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass {
public:
    explicit VulkanRenderPass(const VulkanDevice& device, const VulkanSwapchain& swapchain);
    ~VulkanRenderPass();

    const vk::RenderPass& get() const { return render_pass_; }
    const std::vector<vk::Framebuffer>& framebuffers() const { return framebuffers_; }
    const vk::Semaphore& image_acquired_semaphore() const { return image_acquired_semaphore_; }
    const vk::Framebuffer& current_frame_buffer() const { return framebuffers_[swapchain_current_index_]; }
    uint32_t& swapchain_current_index() { return swapchain_current_index_; }

private:
    // -------------------- Type Definitions -------------------


    // -------------------- Initialization -------------------
    static vk::RenderPass LoadRenderPass(const VulkanDevice& device, const VulkanSwapchain& swapchain);
    static std::vector<vk::Framebuffer> LoadFramebuffers(const VulkanDevice& device,
                                                         const VulkanSwapchain& swapchain,
                                                         const vk::RenderPass& render_pass);
    static vk::Semaphore LoadImageAcquiredSemaphore(const VulkanDevice& device);
    static uint32_t LoadSwapchainCurrentIndex(const vk::Device& device,
                                              const vk::SwapchainKHR& swapchain,
                                              const vk::Semaphore& image_acquired_semaphore);

    // -------------------- Destruction -------------------
    void DestroyFramebuffers();

    // -------------------- Data Members -------------------
    // The logical device responsible for resource allocation and cleanup
    const VulkanDevice& device_;

    // A reference to the swapchain manager
    const VulkanSwapchain& swapchain_;

    // The render pass to which we supply attachments
    vk::RenderPass render_pass_;

    // Memory attachments used by the render pass instance such as the color image buffer
    // and the depth image buffer.
    // In other words, the framebuffers connect these resources to the render pass
    std::vector<vk::Framebuffer> framebuffers_;

    // We need to get an image from the swapchain in order to draw anything
    // Semaphores are used to ensure that all images in the swapchain are currently available
    vk::Semaphore image_acquired_semaphore_;

    // The index of the next available swapchain image
    uint32_t swapchain_current_index_;

    // For logging
    static const std::string kWho;
};

}
}
