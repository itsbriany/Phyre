#pragma once
#include "vulkan_debugger.h"
#include "vulkan_device.h"
#include "vulkan_instance.h"
#include "vulkan_gpu.h"
#include "vulkan_window.h"

namespace Phyre {
namespace Graphics {

class DrawCube {
public:
    DrawCube();
    virtual ~DrawCube();

    // Returns true if the rendering system started correctly
    void Start();

private:
    struct VertexBuffer {
        vk::Buffer buffer;
        vk::DeviceMemory memory;
    };

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
    void LoadShaderModules();
    void LoadVertexBuffer();
    void LoadUniformBuffer();
    void LoadRenderPass();
    void LoadDescriptorPool();
    void LoadDescriptorSets();
    void LoadPipelineCache();
    void LoadPipelineLayout();
    void LoadPipeline();
    void Draw();

    // ------------------- Cleanup Stages ----------------------
    void DestroyShaderModules();
    void DestroyVertexBuffer() const;

    // ------------------------ Helpers -------------------------
    // Returns true when the member dependencies are valid
    bool AreDependenciesValid() const;

    // Load SPIR-V Bytecode from file
    static std::vector<uint32_t> ReadSpirV(const std::string spirv_shader_file_name);

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

    // The shaders which end up getting plugged into the pipeline
    std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stages_;

    // The uniform buffer
    VulkanUniformBuffer* p_uniform_buffer_;

    // The vertex buffer which we will be binding
    VertexBuffer vertex_buffer_;
    vk::VertexInputBindingDescription vertex_input_binding_description_;
    std::array<vk::VertexInputAttributeDescription, 2> vertex_input_attributes_;

    // The render pass used in the graphics pipeline
    VulkanRenderPass *p_render_pass_;

    // Descriptors allow the shaders to access the image and buffer resources
    vk::DescriptorPool descriptor_pool_;

    // Describes the content of a list of descriptor sets
    // Informs the GPU how the data contained in the uniform buffer
    // is mapped to the shader program's uniform variables
    vk::DescriptorSetLayout descriptor_set_layout_;

    // This is what we attach to the pipeline
    std::vector<vk::DescriptorSet> descriptor_sets_;

    // This object is referenced so that the pipeline does not get
    // recreated from scratch all the time
    vk::PipelineCache pipeline_cache_;

    // The graphics pipeline and its layout
    vk::PipelineLayout pipeline_layout_;
    vk::Pipeline pipeline_;

    // ---------------------- Logging ---------------------------
    static const std::string kWho;
};

}
}
