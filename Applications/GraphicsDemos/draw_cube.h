#pragma once
#include <Graphics/vulkan_debugger.h>
#include <Graphics/vulkan_device.h>
#include <Graphics/vulkan_instance.h>
#include <Graphics/vulkan_gpu.h>
#include <Graphics/vulkan_window.h>
#include <Graphics/vulkan_uniform_buffer.h>
#include <Graphics/vulkan_render_pass.h>

namespace Phyre {
namespace Graphics {

class DrawCube {
public:
    DrawCube();
    virtual ~DrawCube();

    // Returns true if the rendering system started correctly
    void Start();

    // Run the game loop
    bool Run() const;

    // Render a frame
    void Draw();

    // Repare to render a frame
    void BeginRender() const;

    // Signals that we have finished rendering a frame
    void EndRender() const;

    // Records the FPS
    void LogFPS() const;

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
    void LoadWindow(float width, float height, const std::string& title);
    void LoadDevice();
    void LoadCommandPool();
    void LoadCommandBuffers();
    void ExecuteBeginCommandBuffer(size_t command_buffer_index);
    void LoadSwapchain();
    void LoadShaderModules();
    void LoadVertexBuffer();
    void LoadUniformBuffer();
    void LoadRenderPass();
    void LoadFrameBuffers();
    void LoadDescriptorPool();
    void LoadDescriptorSets();
    void LoadPipelineCache();
    void LoadPipelineLayout();
    void LoadPipeline();
    void LoadVulkanFence();

    // ------------------- Cleanup Stages ----------------------
    void DestroyShaderModules();
    void DestroyVertexBuffer() const;
    void DestroyFramebuffers();

    // ------------------------ Helpers -------------------------
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

    // Memory attachments used by the render pass instance such as the color image buffer
    // and the depth image buffer.
    // In other words, the framebuffers connect these resources to the render pass
    std::vector<vk::Framebuffer> framebuffers_;

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

    // A fence that we use to know when the rendering is finished
    vk::Fence swapchain_image_available_fence_;

    // ---------------------- Logging ---------------------------
    static const std::string kWho;
};

}
}
