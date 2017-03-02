#include "vulkan_swapchain.h"
#include "vulkan_memory_manager.h"
#include "vulkan_errors.h"
#include "vulkan_window.h"

const std::string Phyre::Graphics::VulkanSwapchainDeprecated::kWho = "[VulkanSwapchain]";

// Initialization pipeline
Phyre::Graphics::VulkanSwapchainDeprecated::VulkanSwapchainDeprecated(const VulkanMemoryManager& memory_manager,
                                                    const VulkanWindow& window,
                                                    const VulkanGPU& gpu,
                                                    const vk::Device& device, 
                                                    uint32_t graphics_queue_family_index, 
                                                    uint32_t presentation_family_index) :
    memory_manager_(memory_manager),
    surface_(window.surface()),
    image_width_(window.width()),
    image_height_(window.height()),
    gpu_(gpu),
    device_(device),
    surface_formats_(InitializeSurfaceFormats(surface_, gpu_.get())),
    preferred_surface_format_(InitializePreferredSurfaceFormat(surface_formats_)),
    surface_capabilities_(InitializeSurfaceCapabilities(surface_, gpu_.get())),
    swapchain_extent_(InitializeSwapchainExtent(window.width(), window.height(), surface_capabilities_)),
    pre_transform_(InitializePreTransform(surface_capabilities_)),
    surface_present_modes_(InitializeSurfacePresentModes(surface_, gpu_.get())),
    preferred_surface_present_mode_(InitializePreferredPresentMode(surface_present_modes_)),
    swapchain_(InitializeSwapchain(device,
                                   surface_,
                                   surface_capabilities_,
                                   preferred_surface_format_,
                                   swapchain_extent_,
                                   pre_transform_,
                                   preferred_surface_present_mode_,
                                   graphics_queue_family_index,
                                   presentation_family_index)),
    swapchain_images_(InitializeSwapchainImages(device_, swapchain_, preferred_surface_format_.format)),
    samples_(vk::SampleCountFlagBits::e1),
    depth_image_(InitializeDepthImage(memory_manager_, gpu_, device_, window.width(), window.height(), samples_)) {
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::VulkanSwapchainDeprecated::~VulkanSwapchainDeprecated() {
    device_.destroyImage(depth_image_.image);
    device_.destroyImageView(depth_image_.image_view);
    device_.freeMemory(depth_image_.device_memory);
    for (const auto& swapchain_image : swapchain_images_) {
        device_.destroyImageView(swapchain_image.image_view);
    }
    device_.destroySwapchainKHR(swapchain_, nullptr);
    Logging::trace("Destroyed", kWho);
}

std::vector<vk::SurfaceFormatKHR> Phyre::Graphics::VulkanSwapchainDeprecated::InitializeSurfaceFormats(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& gpu) {
    uint32_t surface_format_count = 0;
    gpu.getSurfaceFormatsKHR(surface, &surface_format_count, nullptr);

    std::vector<vk::SurfaceFormatKHR> surface_formats(surface_format_count);
    surface_formats = gpu.getSurfaceFormatsKHR(surface);

    if (surface_formats.empty()) {
        Logging::error("No surface formats detected", kWho);
    }

    return surface_formats;
}

vk::SurfaceFormatKHR Phyre::Graphics::VulkanSwapchainDeprecated::InitializePreferredSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surface_formats) {
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

vk::Extent2D Phyre::Graphics::VulkanSwapchainDeprecated::InitializeSwapchainExtent(uint32_t width, uint32_t height, const vk::SurfaceCapabilitiesKHR& surface_capabilities) {
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

vk::SurfaceTransformFlagBitsKHR Phyre::Graphics::VulkanSwapchainDeprecated::InitializePreTransform(const vk::SurfaceCapabilitiesKHR& surface_capabilities) {
    vk::SurfaceTransformFlagBitsKHR pre_transform;
    if (surface_capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
        pre_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    } else {
        pre_transform = surface_capabilities.currentTransform;
    }
    return pre_transform;
}

vk::SurfaceCapabilitiesKHR Phyre::Graphics::VulkanSwapchainDeprecated::InitializeSurfaceCapabilities(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& gpu) {
    return gpu.getSurfaceCapabilitiesKHR(surface);
}

Phyre::Graphics::VulkanSwapchainDeprecated::PresentModes Phyre::Graphics::VulkanSwapchainDeprecated::InitializeSurfacePresentModes(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& gpu) {
    uint32_t present_modes_count = 0;
    gpu.getSurfacePresentModesKHR(surface, &present_modes_count, nullptr);

    PresentModes surface_present_modes(present_modes_count);
    surface_present_modes = gpu.getSurfacePresentModesKHR(surface);
    if (surface_present_modes.empty()) {
        Logging::error("No surface presentation modes detected", kWho);
    }
    return surface_present_modes;
}

vk::PresentModeKHR Phyre::Graphics::VulkanSwapchainDeprecated::InitializePreferredPresentMode(const PresentModes& surface_present_modes) {
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

vk::SwapchainKHR Phyre::Graphics::VulkanSwapchainDeprecated::InitializeSwapchain(const vk::Device& device,
                                                                       const vk::SurfaceKHR& surface, 
                                                                       const vk::SurfaceCapabilitiesKHR& surface_capabilities,
                                                                       const vk::SurfaceFormatKHR& surface_format,
                                                                       const vk::Extent2D& swapchain_extent,
                                                                       const vk::SurfaceTransformFlagBitsKHR& pre_transform,
                                                                       const vk::PresentModeKHR& surface_present_mode,
                                                                       uint32_t graphics_queue_family_index,
                                                                       uint32_t presentation_queue_family_index) {
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

    if (graphics_queue_family_index != presentation_queue_family_index) {
        // When the graphics and present queues are from different queue families,
        // we either have to explicitly transfer ownership of images between
        // the queues, or we have to share image resources between them
        std::array<uint32_t, 2> queueFamilyIndices = { graphics_queue_family_index, presentation_queue_family_index };
        swapchain_create_info.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchain_create_info.queueFamilyIndexCount = queueFamilyIndices.size();
        swapchain_create_info.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    
    vk::SwapchainKHR swapchain;
    vk::Result result = device.createSwapchainKHR(&swapchain_create_info, nullptr, &swapchain);
    if (ErrorCheck(result, kWho)) {
        return swapchain;
    }

    std::string error_message = "Failed to initialize swapchain";
    Logging::fatal(error_message, kWho);
    throw std::runtime_error(error_message);
}

Phyre::Graphics::VulkanSwapchainDeprecated::SwapchainImageVector Phyre::Graphics::VulkanSwapchainDeprecated::InitializeSwapchainImages(const vk::Device& device,
                                                                                                                   const vk::SwapchainKHR& swapchain,
                                                                                                                   const vk::Format& format) {
    uint32_t swapchain_image_count = 0;
    device.getSwapchainImagesKHR(swapchain, &swapchain_image_count, nullptr);

    std::vector<vk::Image> image_buffers(swapchain_image_count);
    vk::Result result = device.getSwapchainImagesKHR(swapchain, &swapchain_image_count, image_buffers.data());
    if (!ErrorCheck(result, kWho)) {
        std::string error_message = "Could not initialize swapchain images";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }

    ImageViewVector image_views;
    for (const vk::Image& image : image_buffers) {
        vk::ImageViewCreateInfo color_image_view_create_info;

        color_image_view_create_info.setImage(image);
        color_image_view_create_info.setViewType(vk::ImageViewType::e2D);
        color_image_view_create_info.setFormat(format);

        // RGBA
        vk::ComponentMapping component_mapping;
        component_mapping.setR(vk::ComponentSwizzle::eR);
        component_mapping.setG(vk::ComponentSwizzle::eG);
        component_mapping.setB(vk::ComponentSwizzle::eB);
        component_mapping.setA(vk::ComponentSwizzle::eA);
        color_image_view_create_info.setComponents(component_mapping);

        // Subresourcerange
        vk::ImageSubresourceRange image_subresource_range;
        image_subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
        image_subresource_range.setBaseMipLevel(0);
        image_subresource_range.setLevelCount(1);
        image_subresource_range.setBaseArrayLayer(0);
        image_subresource_range.setLayerCount(1);
        color_image_view_create_info.setSubresourceRange(image_subresource_range);

        vk::ImageView image_view;
        result = device.createImageView(&color_image_view_create_info, nullptr, &image_view);
        if (!ErrorCheck(result, kWho)) {
            std::string error_message = "Failed to create image view";
            Logging::fatal(error_message, kWho);
            throw std::runtime_error(error_message);
        }
       
        image_views.emplace_back(image_view);
    }

    SwapchainImageVector swapchain_images;
    for (uint32_t i = 0; i < swapchain_image_count; ++i) {
        SwapchainImage swapchain_image;
        swapchain_image.image = image_buffers[i];
        swapchain_image.image_view = image_views[i];
        swapchain_images.emplace_back(swapchain_image);
    }
    return swapchain_images;
}

Phyre::Graphics::VulkanSwapchainDeprecated::DepthImage Phyre::Graphics::VulkanSwapchainDeprecated::InitializeDepthImage(const VulkanMemoryManager& memory_manager,
                                                                                                      const VulkanGPU& gpu,
                                                                                                      const vk::Device& device,
                                                                                                      uint32_t width,
                                                                                                      uint32_t height,
                                                                                                      vk::SampleCountFlagBits samples) {
    // Create a depth buffer image so that we can eventually have 3D rendering.
    vk::ImageCreateInfo image_create_info;
    vk::Format depth_format = vk::Format::eD16Unorm;
    vk::FormatProperties format_properties;
    gpu.get().getFormatProperties(depth_format, &format_properties);

    // Make sure the GPU supports depth
    if (format_properties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
        image_create_info.setTiling(vk::ImageTiling::eLinear);
    } else if (format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
        image_create_info.setTiling(vk::ImageTiling::eOptimal);
    } else {
        /* Try other depth formats? */
        std::string error_message = "VK_FORMAT_D16_UNORM Unsupported";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }

    image_create_info.setImageType(vk::ImageType::e2D);
    image_create_info.setFormat(depth_format);

    vk::Extent3D extent;
    extent.setWidth(width);
    extent.setHeight(height);
    extent.setDepth(1);
    image_create_info.setExtent(extent);

    image_create_info.setMipLevels(1);
    image_create_info.setArrayLayers(1);
    image_create_info.setSamples(samples);
    image_create_info.setInitialLayout(vk::ImageLayout::eUndefined);
    image_create_info.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);
    image_create_info.setQueueFamilyIndexCount(0);
    image_create_info.setPQueueFamilyIndices(nullptr);
    image_create_info.setSharingMode(vk::SharingMode::eExclusive);

    vk::Image depth_image;
    vk::Result result = device.createImage(&image_create_info, nullptr, &depth_image);
    if (!ErrorCheck(result, kWho)) {
        std::string error_message = "Could not create image";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }

    // Gather memory allocation requirements for non-sparse images
    // We do this because we still need to figure out how much memory to allocate.
    // There may be alignment constraints placed by the GPU hardware, so we need to be aware of this.
    vk::MemoryRequirements memory_requirements;
    device.getImageMemoryRequirements(depth_image, &memory_requirements);
    
    // Now that we know about the memory constraints, we can now allocate memory for our image
    /* Use the memory properties to determine the type of memory required */
    vk::MemoryAllocateInfo memory_allocate_info;
    vk::MemoryPropertyFlagBits requirements_mask = vk::MemoryPropertyFlagBits::eDeviceLocal;
    if(!VulkanMemoryManager::CanFindMemoryTypeFromProperties(gpu, memory_requirements.memoryTypeBits, requirements_mask, memory_allocate_info.memoryTypeIndex)) {
        std::string error_message = "Could not satisfy memory requirements for image";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }
    
    memory_allocate_info.setAllocationSize(memory_requirements.size);
    memory_allocate_info.setMemoryTypeIndex(memory_allocate_info.memoryTypeIndex);

    vk::DeviceMemory device_memory;
    result = device.allocateMemory(&memory_allocate_info, nullptr, &device_memory);
    if (!ErrorCheck(result, kWho)) {
        std::string error_message = "Failed to allocate device memory for image";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }

    // Finally, we may bind the memory to our depth image buffer
    device.bindImageMemory(depth_image, device_memory, 0);

    // Give it a view
    vk::ImageViewCreateInfo depth_image_view_create_info;
    depth_image_view_create_info.setImage(depth_image);
    depth_image_view_create_info.setFormat(depth_format);
    
    // RGBA
    vk::ComponentMapping component_mapping;
    component_mapping.setR(vk::ComponentSwizzle::eR);
    component_mapping.setG(vk::ComponentSwizzle::eG);
    component_mapping.setB(vk::ComponentSwizzle::eB);
    component_mapping.setA(vk::ComponentSwizzle::eA);
    depth_image_view_create_info.setComponents(component_mapping);

    // Subresources
    vk::ImageSubresourceRange image_subresource_range;
    image_subresource_range.setAspectMask(vk::ImageAspectFlagBits::eDepth); // TODO Might want add stencil as well?
    image_subresource_range.setBaseMipLevel(0);
    image_subresource_range.setLevelCount(1);
    image_subresource_range.setBaseArrayLayer(0);
    image_subresource_range.setLayerCount(1);
    depth_image_view_create_info.setSubresourceRange(image_subresource_range);
    depth_image_view_create_info.setViewType(vk::ImageViewType::e2D);

    vk::ImageView depth_image_view;
    result = device.createImageView(&depth_image_view_create_info, nullptr, &depth_image_view);
    if (!ErrorCheck(result, kWho)) {
        std::string error_message = "Failed to initialize depth image view";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }

    return DepthImage(depth_image, depth_image_view, depth_format, device_memory);
}
