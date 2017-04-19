#pragma once
#include <Configuration/provider.h>
#include <Graphics/vulkan_debugger.h>
#include <Graphics/vulkan_device.h>
#include <Graphics/vulkan_instance.h>
#include <Graphics/vulkan_gpu.h>
#include <Graphics/vulkan_window.h>
#include <Graphics/vulkan_render_pass.h>
#include <Graphics/uniform_buffer.h>
#include <Input/handler.h>

namespace Phyre {
namespace Graphics {

class DrawCube : public Input::Handler {
public:
    //---------------------- Type Definitions -------------------------
    typedef Handler BaseClass;

    //---------------------- Construction/Destruction -----------------
    static std::shared_ptr<DrawCube> Create(int argc, const char* argv[]);
    ~DrawCube();

    //---------------------- Base Class Overrides ---------------------

    void OnFramebufferResize(int width, int height) override;
    void OnMousePositionUpdate(double x, double y) override;
    void OnKeyRelease(Input::Key key, int mods) override;
    void OnMousePress(Input::Mouse mouse_button, int mods) override;

    //---------------------- Interface --------------------------------

    // Run the game loop
    bool Run() const;

    // Render a frame
    void Draw();

    // Repare to render a frame
    void BeginRender() const;

    // Signals that we have finished rendering a frame
    void EndRender();

    // Gracefully clean up the application by waiting for 
    // all the rendering operations to finish
    void Stop() const;

    // Records the FPS
    static void LogFPS();

    // Get the input window
    Input::Window* input_window() const { return p_vk_window_->window(); }

private:
    // TODO This could be refactored into some factory pattern such that we can
    // only create shared_ptrs from this class.
    DrawCube(int argc, const char* argv[]);

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
    void UpdateUniformBuffers() const;
    void LoadRenderPass();
    void LoadFrameBuffers();
    void LoadDescriptorPool();
    void LoadDescriptorSets();
    void LoadPipelineCache();
    void LoadPipelineLayout();
    void LoadPipeline();
    void LoadSemaphores();

    // Loads all of the previous steps
    void LoadGraphics();

    // ------------------- Cleanup Stages ----------------------
    void DestroyShaderModules();
    void DestroyVertexBuffer() const;
    void DestroyFramebuffers();

    // ------------------- Event-Driven Stages ------------------
    void ReloadSwapchain();

    // ---------------------- Data Types ------------------------
    VulkanInstance instance_;
    VulkanDebugger debugger_;
    std::vector<VulkanGPU> gpus_;
    
    // Points to one of the GPUs in the GPU vector, so its lifetime depends on the GPU vector
    VulkanGPU* p_active_gpu_; 

    // Points to a window where we present our rendered data to
    std::shared_ptr<VulkanWindow> p_vk_window_;

    // Points to a logical device which can be used to access queues to which we submit command buffers
    VulkanDevice* p_device_;

    // The command pools from which we may allocate command buffers from
    vk::CommandPool command_pool_;

    // A single command buffer to which we may send commands to
    CommandBuffers command_buffers_;

    // A swapchain which helps manage image buffers
    std::unique_ptr<VulkanSwapchain> p_swapchain_;

    // The shaders which end up getting plugged into the pipeline
    std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stages_;

    // The uniform buffer
    std::unique_ptr<UniformBuffer> p_uniform_buffer_;

    // The vertex buffer which we will be binding
    VertexBuffer vertex_buffer_;
    vk::VertexInputBindingDescription vertex_input_binding_description_;
    std::array<vk::VertexInputAttributeDescription, 2> vertex_input_attributes_;

    // The render pass used in the graphics pipeline
    std::unique_ptr<VulkanRenderPass> p_render_pass_;

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

    // Signals when the command buffers have finished executing
    vk::Semaphore render_finished_semaphore_;

    // The configuration provider for loading resources
    std::unique_ptr<Configuration::Provider> p_provider_;

    // The the name of this application
    std::string target_;
    
    // ---------------------- Logging ---------------------------
    static const std::string kWho;
};

}
}
