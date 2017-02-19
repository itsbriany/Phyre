#include "vulkan_swapchain.h"
#include "vulkan_errors.h"
#include "vulkan_window.h"
#include "vulkan_device.h"

const std::string Phyre::Graphics::VulkanSwapchain::kWho = "[VulkanSwapchain]";

// Initialization pipeline
Phyre::Graphics::VulkanSwapchain::VulkanSwapchain(const VulkanWindow& window, const VulkanDevice& device) :
    surface_(window.GetSurfaceReference()),
    device_(device),
    surface_formats_(InitializeSurfaceFormats(surface_, device.GpuReference())),
    preferred_surface_format_(InitializePreferredSurfaceFormat(surface_formats_)),
    surface_capabilities_(InitializeSurfaceCapabilities(surface_, device.GpuReference())),
    swapchain_extent_(InitializeSwapchainExtent(window.width(), window.height(), surface_capabilities_)),
    pre_transform_(InitializePreTransform(surface_capabilities_)),
    surface_present_modes_(InitializeSurfacePresentModes(surface_, device.GpuReference())),
    preferred_surface_present_mode_(InitializePreferredPresentMode(surface_present_modes_)),
    swapchain_(InitializeSwapchain(device,
                                   surface_,
                                   surface_capabilities_,
                                   preferred_surface_format_,
                                   swapchain_extent_,
                                   pre_transform_,
                                   preferred_surface_present_mode_))
{}

std::vector<vk::SurfaceFormatKHR> Phyre::Graphics::VulkanSwapchain::InitializeSurfaceFormats(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& gpu) {
    uint32_t surface_format_count = 0;
    gpu.getSurfaceFormatsKHR(surface, &surface_format_count, nullptr);

    std::vector<vk::SurfaceFormatKHR> surface_formats(surface_format_count);
    surface_formats = gpu.getSurfaceFormatsKHR(surface);

    if (surface_formats.empty()) {
        Logging::error("No surface formats detected", kWho);
    }

    return surface_formats;
}

vk::SurfaceFormatKHR Phyre::Graphics::VulkanSwapchain::InitializePreferredSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surface_formats) {
    if (surface_formats.empty()) {
        Logging::warning("No surface formats detected!", kWho);
    }

    // The first image format is the preferred one
    vk::SurfaceFormatKHR surface_format;
    const vk::Format default_image_format = vk::Format::eB8G8R8A8Unorm;
    if (surface_formats.size() == 1 && surface_formats.front().format == vk::Format::eUndefined) {
        surface_format.format = default_image_format;
    } else {
        surface_format.format = surface_formats.front().format;
    }

    surface_format.colorSpace = surface_formats.front().colorSpace;
    return surface_format;
}

vk::Extent2D Phyre::Graphics::VulkanSwapchain::InitializeSwapchainExtent(uint32_t width, uint32_t height, const vk::SurfaceCapabilitiesKHR& surface_capabilities) {
    // Width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    vk::Extent2D extent;
    if (surface_capabilities.currentExtent.width == 0xFFFFFFFF) {
        extent.width = width;
        extent.height = height;

        // We make a sanity check to optimize the extent of our swapchain buffers
        if (extent.width < surface_capabilities.minImageExtent.width) {
            extent.width = surface_capabilities.minImageExtent.width;
        } else if (extent.width > surface_capabilities.maxImageExtent.width) {
            extent.width = surface_capabilities.maxImageExtent.width;
        }

        if (extent.height < surface_capabilities.minImageExtent.height) {
            extent.height = surface_capabilities.minImageExtent.height;
        } else if (extent.height > surface_capabilities.maxImageExtent.height) {
            extent.height = surface_capabilities.maxImageExtent.height;
        }
    } else {
        // If the surface size is defined, the swap chain size must match
        extent = surface_capabilities.currentExtent;
    }
    return extent;
}

vk::SurfaceTransformFlagBitsKHR Phyre::Graphics::VulkanSwapchain::InitializePreTransform(const vk::SurfaceCapabilitiesKHR& surface_capabilities) {
    vk::SurfaceTransformFlagBitsKHR pre_transform;
    if (surface_capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
        pre_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    } else {
        pre_transform = surface_capabilities.currentTransform;
    }
    return pre_transform;
}

vk::SurfaceCapabilitiesKHR Phyre::Graphics::VulkanSwapchain::InitializeSurfaceCapabilities(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& gpu) {
    return gpu.getSurfaceCapabilitiesKHR(surface);
}

Phyre::Graphics::VulkanSwapchain::PresentModes Phyre::Graphics::VulkanSwapchain::InitializeSurfacePresentModes(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& gpu) {
    uint32_t present_modes_count = 0;
    gpu.getSurfacePresentModesKHR(surface, &present_modes_count, nullptr);

    PresentModes surface_present_modes(present_modes_count);
    surface_present_modes = gpu.getSurfacePresentModesKHR(surface);
    if (surface_present_modes.empty()) {
        Logging::error("No surface presentation modes detected", kWho);
    }
    return surface_present_modes;
}

vk::PresentModeKHR Phyre::Graphics::VulkanSwapchain::InitializePreferredPresentMode(const PresentModes& surface_present_modes) {
    // Tears when the app misses, but does not tear when the app is fast enough
    if (std::find(surface_present_modes.cbegin(), surface_present_modes.cend(), vk::PresentModeKHR::eFifoRelaxed) != surface_present_modes.cend()) {
        return vk::PresentModeKHR::eFifoRelaxed;
    }
    
    // Tearning is never observed
    if (std::find(surface_present_modes.cbegin(), surface_present_modes.cend(), vk::PresentModeKHR::eFifo) != surface_present_modes.cend()) {
        return vk::PresentModeKHR::eFifo;
    }

    // Never tears
    if (std::find(surface_present_modes.cbegin(), surface_present_modes.cend(), vk::PresentModeKHR::eMailbox) != surface_present_modes.cend()) {
        return vk::PresentModeKHR::eMailbox;
    }

    // Tears because the presentation requests are immediately rendered
    return vk::PresentModeKHR::eImmediate;
}

vk::SwapchainKHR Phyre::Graphics::VulkanSwapchain::InitializeSwapchain(const VulkanDevice& device, 
                                                                       const vk::SurfaceKHR& surface, 
                                                                       const vk::SurfaceCapabilitiesKHR& surface_capabilities,
                                                                       const vk::SurfaceFormatKHR& surface_format,
                                                                       const vk::Extent2D& swapchain_extent,
                                                                       const vk::SurfaceTransformFlagBitsKHR& pre_transform,
                                                                       const vk::PresentModeKHR& surface_present_mode) {
    vk::SwapchainCreateInfoKHR swapchain_create_info;
    swapchain_create_info.setSurface(surface);
    swapchain_create_info.setMinImageCount(surface_capabilities.minImageCount); // Minimum is generally double buffering
    swapchain_create_info.setImageFormat(surface_format.format);
    swapchain_create_info.setImageColorSpace(surface_format.colorSpace);
    swapchain_create_info.setImageExtent(swapchain_extent);
    swapchain_create_info.setImageArrayLayers(1); // For simplicity, we will only use one for now
    swapchain_create_info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst); // How will the application use the swapchain's presentable images?
    swapchain_create_info.setImageSharingMode(vk::SharingMode::eExclusive); // Let's prevent other resources from accessing this swap chain for now
    swapchain_create_info.setQueueFamilyIndexCount(0); // If we are using VK_SHARING_MODE_CONCURRENT, then here we can specify how many queue families we want to use
    swapchain_create_info.setPQueueFamilyIndices(nullptr); // Only use when VK_SHARING_MODE_CONCURRENT
    swapchain_create_info.setPreTransform(pre_transform);
    swapchain_create_info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
    swapchain_create_info.setPresentMode(surface_present_mode);
    swapchain_create_info.setClipped(true); // We are allowd to discard rendering operations affecting regions of the surface which are not visible
    swapchain_create_info.setOldSwapchain(nullptr);

    if (device.graphics_queue_family_index() != device.presentation_queue_family_index()) {
        // When the graphics and present queues are from different queue families,
        // we either have to explicitly transfer ownership of images between
        // the queues, or we have to share image resources between them
        std::array<uint32_t, 2> queueFamilyIndices = { device.graphics_queue_family_index(), device.presentation_queue_family_index() };
        swapchain_create_info.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchain_create_info.queueFamilyIndexCount = queueFamilyIndices.size();
        swapchain_create_info.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    
    vk::SwapchainKHR swapchain;
    vk::Result result = device.DeviceReference().createSwapchainKHR(&swapchain_create_info, nullptr, &swapchain);
    if (ErrorCheck(result, kWho)) {
        return swapchain;
    }

    std::string error_message = "Failed to initialize swapchain";
    Logging::fatal(error_message, kWho);
    throw std::runtime_error(error_message);
}
