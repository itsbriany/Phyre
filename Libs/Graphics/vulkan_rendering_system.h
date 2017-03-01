#pragma once
#include "rendering_system_interface.h"
#include "vulkan_debugger.h"
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
    // ------------------- Render System Stages -----------------
    // This will only be enabled in debug mode and when the debug layers are active
    void StartDebugger();
    void LoadGPUs();
    void LoadWindow(uint32_t width, uint32_t height);

    // ---------------------- Data Types ------------------------
    VulkanInstance instance_;
    VulkanDebugger debugger_;
    std::vector<VulkanGPU> gpus_;
    
    // Points to one of the GPUs in the GPU vector, so its lifetime depends on the GPU vector
    VulkanGPU* p_active_gpu_; 

    // Points to a window where we present our rendered data to
    VulkanWindow* p_window_;

    // ---------------------- Logging ---------------------------
    static const std::string kWho;
};

}
}
