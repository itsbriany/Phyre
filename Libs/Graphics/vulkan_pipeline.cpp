#include "vulkan_pipeline.h"
#include "vulkan_errors.h"

const std::string Phyre::Graphics::VulkanPipeline::kWho = "[VulkanPipeline]";


Phyre::Graphics::VulkanPipeline::VulkanPipeline(const VulkanDevice& device,
                                                const VulkanSwapchain& swapchain) :
    device_(device),
    swapchain_(swapchain),
    descriptor_set_count_(1),
    // pipeline_layout_(InitializePipelineLayout(device_.get(), descriptor_set_layout_, descriptor_set_count_)),
    render_pass_(device_, swapchain_)
    // pipeline_(InitializeGraphicsPipeline(device_, render_pass_, pipeline_layout_))
    {
    Logging::trace("Instantiated", kWho);
    // render_pass_.BeginRenderPass(command_buffer_manager_.command_buffer(), *this, graphics_queue, presentation_queue);
}

Phyre::Graphics::VulkanPipeline::~VulkanPipeline() {   
    // device_.device().destroyPipeline(pipeline_);
    // device_.get().destroyPipelineLayout(pipeline_layout_);
    Logging::trace("Destroyed", kWho);
}

