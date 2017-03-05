#pragma once
#include "rendering_system_interface.h"
#include "vulkan_debugger.h"
#include "vulkan_device.h"
#include "vulkan_instance.h"
#include "vulkan_gpu.h"
#include "vulkan_window.h"

namespace Phyre {
namespace Graphics {

class VulkanRenderingSystem : public RenderingSystemInterface {
public:
    VulkanRenderingSystem();
    virtual ~VulkanRenderingSystem();

    // Returns true if the rendering system started correctly
    void Start() override;

private:
    // ------------------- Type Definitions ---------------------
    typedef std::vector<vk::CommandBuffer> CommandBuffers;

    // ------------------- Render System Stages -----------------
    // This will only be enabled in debug mode and when the debug layers are active
    void StartDebugger();
    void LoadGPUs();
    void LoadWindow(uint32_t width, uint32_t height);
    void LoadDevice();
    void LoadCommandPool();
    void LoadCommandBuffers();
    void ExecuteBeginCommandBuffer(size_t command_buffer_index);
    void LoadSwapchain();
    void LoadPipeline();

    // ------------------------ Helpers -------------------------
    // Returns true when the member dependencies are valid
    bool AreDependenciesValid() const;

    // ---------------------- Data Types ------------------------
    VulkanInstance instance_;
    VulkanDebugger debugger_;
    std::vector<VulkanGPU> gpus_;
    
    // Points to one of the GPUs in the GPU vector, so its lifetime depends on the GPU vector
    VulkanGPU* p_active_gpu_; 

    // Points to a window where we present our rendered data to
    VulkanWindow* p_window_;

    // Points to a logical device which can be used to access queues to which we submit command buffers
    VulkanDevice* p_device_;

    // The command pools from which we may allocate command buffers from
    vk::CommandPool command_pool_;

    // A single command buffer to which we may send commands to
    CommandBuffers command_buffers_;

    // A swapchain which helps manage image buffers
    VulkanSwapchain* p_swapchain_;

    // The pipeline
    VulkanPipeline* p_pipeline_;

    // ---------------------- Logging ---------------------------
    static const std::string kWho;
};

}
}
