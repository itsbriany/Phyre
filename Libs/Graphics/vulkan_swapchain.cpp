#include "vulkan_swapchain.h"
#include "vulkan_memory_manager.h"
#include "vulkan_errors.h"
#include "vulkan_window.h"
#include "vulkan_device.h"

const std::string Phyre::Graphics::VulkanSwapchain::kWho = "[VulkanSwapchain]";

// Initialization pipeline
Phyre::Graphics::VulkanSwapchain::VulkanSwapchain(const VulkanDevice& device, const VulkanWindow& window) :
    surface_(window.surface()),
    image_width_(window.width()),
    image_height_(window.height()),
    device_(device),
    swapchain_extent_(InitializeSwapchainExtent(window)),
    pre_transform_(InitializePreTransform(window.surface_capabilities())),
    swapchain_(InitializeSwapchain(device_, window, swapchain_extent_, pre_transform_)),
    swapchain_images_(InitializeSwapchainImages(device_.device(), swapchain_, window.preferred_surface_format().format)),
    samples_(vk::SampleCountFlagBits::e1),
    depth_image_(InitializeDepthImage(device_, window.width(), window.height(), samples_)) {
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::VulkanSwapchain::~VulkanSwapchain() {
    device_.device().destroyImage(depth_image_.image);
    device_.device().destroyImageView(depth_image_.image_view);
    device_.device().freeMemory(depth_image_.device_memory);
    for (const auto& swapchain_image : swapchain_images_) {
        device_.device().destroyImageView(swapchain_image.image_view);
    }
    device_.device().destroySwapchainKHR(swapchain_, nullptr);
    Logging::trace("Destroyed", kWho);
}

vk::SwapchainKHR Phyre::Graphics::VulkanSwapchain::InitializeSwapchain(const VulkanDevice& device,
                                                                       const VulkanWindow& window, 
                                                                       const vk::Extent2D& extent,
                                                                       const vk::SurfaceTransformFlagBitsKHR& pre_transform) {
    uint32_t desired_number_swapchain_images = window.surface_capabilities().minImageCount;
    vk::SwapchainCreateInfoKHR info;
    info.setSurface(window.surface());
    info.setMinImageCount(desired_number_swapchain_images); // Minimum is generally double buffering
    info.setImageFormat(window.preferred_surface_format().format);
    info.setImageExtent(extent);
    info.setPreTransform(pre_transform);
    info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
    info.setImageArrayLayers(1);
    info.setPresentMode(window.preferred_present_mode());
    info.setOldSwapchain(nullptr);
    info.setClipped(false); // We are not allowed to discard rendering operations affecting regions of the surface which are not visible
    info.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear);
    info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst);
    info.setImageSharingMode(vk::SharingMode::eExclusive); // If we are using VK_SHARING_MODE_CONCURRENT, then here we can specify how many queue families we want to use
    info.setQueueFamilyIndexCount(0);
    info.setPQueueFamilyIndices(nullptr);
    if (device.graphics_queue_family_index() != device.presentation_queue_family_index()) {
        // When the graphics and present queues are from different queue families,
        // we either have to explicitly transfer ownership of images between
        // the queues, or we have to share image resources between them
        std::array<uint32_t, 2> queueFamilyIndices = { device.graphics_queue_family_index(), device.presentation_queue_family_index() };
        info.setImageSharingMode(vk::SharingMode::eConcurrent);
        info.setQueueFamilyIndexCount(queueFamilyIndices.size());
        info.setPQueueFamilyIndices(queueFamilyIndices.data());
    }
    return device.device().createSwapchainKHR(info);
}

vk::Extent2D Phyre::Graphics::VulkanSwapchain::InitializeSwapchainExtent(const VulkanWindow& window) {
    // Width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    vk::Extent2D extent;
    const vk::SurfaceCapabilitiesKHR& surface_capabilities = window.surface_capabilities();
    if (surface_capabilities.currentExtent.width == 0xFFFFFFFF) {
        extent.width = window.width();
        extent.height = window.height();

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

Phyre::Graphics::VulkanSwapchain::SwapchainImageVector Phyre::Graphics::VulkanSwapchain::InitializeSwapchainImages(const vk::Device& device,
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

Phyre::Graphics::VulkanSwapchain::DepthImage Phyre::Graphics::VulkanSwapchain::InitializeDepthImage(const VulkanDevice& device,
                                                                                                    uint32_t width,
                                                                                                    uint32_t height,
                                                                                                    vk::SampleCountFlagBits samples) {
    // Create a depth buffer image so that we can eventually have 3D rendering.
    vk::ImageCreateInfo image_create_info;
    vk::Format depth_format = vk::Format::eD16Unorm;
    vk::FormatProperties format_properties;
    device.GpuReference().get().getFormatProperties(depth_format, &format_properties);

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

    vk::Image depth_image = device.device().createImage(image_create_info);
    

    // Gather memory allocation requirements for non-sparse images
    // We do this because we still need to figure out how much memory to allocate.
    // There may be alignment constraints placed by the GPU hardware, so we need to be aware of this.
    vk::MemoryRequirements memory_requirements;
    device.device().getImageMemoryRequirements(depth_image, &memory_requirements);
    
    // Now that we know about the memory constraints, we can now allocate memory for our image
    /* Use the memory properties to determine the type of memory required */
    vk::MemoryAllocateInfo memory_allocate_info;
    vk::MemoryPropertyFlagBits requirements_mask = vk::MemoryPropertyFlagBits::eDeviceLocal;
    if(!VulkanMemoryManager::CanFindMemoryTypeFromProperties(device.GpuReference(), memory_requirements.memoryTypeBits, requirements_mask, memory_allocate_info.memoryTypeIndex)) {
        std::string error_message = "Could not satisfy memory requirements for image";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }
    
    memory_allocate_info.setAllocationSize(memory_requirements.size);
    memory_allocate_info.setMemoryTypeIndex(memory_allocate_info.memoryTypeIndex);

    vk::DeviceMemory device_memory = device.device().allocateMemory(memory_allocate_info);

    // Finally, we may bind the memory to our depth image buffer
    device.device().bindImageMemory(depth_image, device_memory, 0);

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

    vk::ImageView depth_image_view = device.device().createImageView(depth_image_view_create_info);

    return DepthImage(depth_image, depth_image_view, depth_format, device_memory);
}
