#pragma once
#include <vulkan.hpp>

namespace Phyre {
namespace Graphics {

// TODO Currently, the functions are stateless. It would be worthwhile to manage state
// such as command pools and buffers, this way, we can simply submit to them and create a 
// new command pool when necessary. Still need to figure out a way to identify command buffers though.
class DeviceManager;
class CommandBufferManager {
public:
    //--------------- Type definitions --------------------
    typedef std::vector<vk::CommandBuffer> CommandBufferVector;

    // --------------- Construction/Destruction ------------
    CommandBufferManager(const DeviceManager& device);
    ~CommandBufferManager();

private:
    // --------------------------- Initialization ------------------------
    // Throws a runtime exception if the command pool failed to instantiate
    static vk::CommandPool InitializeCommandPool(const vk::Device& device, uint32_t graphics_queue_family_index);

    // Returns true if a command buffer could be created from the logical device
    static CommandBufferVector InitializeCommandBuffers(const vk::Device& device, const vk::CommandPool& command_pool);

    // --------------------- Internal methods ----------------------------
    // Input arguments are passed by reference, the info structures are optionals
    void AllocateCommandBuffer(CommandBufferVector& command_buffers, vk::CommandBufferAllocateInfo* info) const;
    void BeginCommandBuffer(vk::CommandBuffer& command_buffer, vk::CommandBufferBeginInfo* info) const;
    static void EndCommandBuffer(vk::CommandBuffer& command_buffer);
    void SubmitToQueue(const vk::Queue& queue, const vk::CommandBuffer& command_buffer, const vk::SubmitInfo* info, const vk::Fence& fence) const;

    // ------------------- Data Members -------------------------------
    // A reference to the logical device which owns us
    const DeviceManager& device_manager_;

    // The command pool from which we create command buffers
    vk::CommandPool command_pool_;

    // The command buffer to which we send Vulkan commands to
    CommandBufferVector command_buffers_;

    // For logging
    static const std::string kWho;
};

}
}
