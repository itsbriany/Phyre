#include "command_buffer_manager.h"
#include "device_manager.h"
#include "logging.h"
#include "vulkan_errors.h"

const std::string Phyre::Graphics::CommandBufferManager::kWho = "[CommandBufferManager]";

Phyre::Graphics::CommandBufferManager::CommandBufferManager(const DeviceManager& device) :
    device_manager_(device),
    command_pool_(InitializeCommandPool(device_manager_.device(), device_manager_.graphics_queue_family_index())),
    command_buffers_(InitializeCommandBuffers(device_manager_.device(), command_pool_)) {
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::CommandBufferManager::~CommandBufferManager() {
    device_manager_.device().destroyCommandPool(command_pool_, nullptr);
    Logging::trace("Destroyed", kWho);
}

vk::CommandPool Phyre::Graphics::CommandBufferManager::InitializeCommandPool(const vk::Device& device, uint32_t graphics_queue_family_index) {
    /**
    * Command buffers reside in command buffer pools.
    * This is necessary for allocating command buffers
    * because memory is coarsly allocated in large chunks between the CPU and GPU.
    */
    vk::CommandPoolCreateInfo command_pool_create_info(vk::CommandPoolCreateFlags(), graphics_queue_family_index);
    vk::CommandPool command_pool;
    vk::Result create_command_pool_result = device.createCommandPool(&command_pool_create_info, nullptr, &command_pool);

    if (create_command_pool_result != vk::Result::eSuccess) {
        Logging::fatal("Could not initialize command pool from logical device", kWho);
    }
    return command_pool;
}

Phyre::Graphics::CommandBufferManager::CommandBufferVector 
Phyre::Graphics::CommandBufferManager::InitializeCommandBuffers(const vk::Device& device, const vk::CommandPool& command_pool) {
    // Allocate the command buffers
    uint32_t command_buffer_count = 1; // Only allocate one command buffer for now
    vk::CommandBufferAllocateInfo command_buffer_allocate_info(command_pool, vk::CommandBufferLevel::ePrimary, command_buffer_count);
    CommandBufferVector command_buffers = device.allocateCommandBuffers(command_buffer_allocate_info);
    if (command_buffers.empty()) {
        Logging::fatal("Failed to allocate command buffers", kWho);
    }

    return command_buffers;
}

void
Phyre::Graphics::CommandBufferManager::AllocateCommandBuffer(CommandBufferVector& command_buffers, vk::CommandBufferAllocateInfo* info) const {
    if (info) {
        command_buffers = device_manager_.device().allocateCommandBuffers(*info);
        if (command_buffers.empty()) {
            Logging::warning("Could not allocate command buffers given the info", kWho);
        }
        return;
    }

    vk::CommandBufferAllocateInfo command_buffer_allocate_info;
    uint32_t command_buffer_count = 1;
    command_buffer_allocate_info.setCommandPool(command_pool_);
    command_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);
    command_buffer_allocate_info.setCommandBufferCount(command_buffer_count);

    command_buffers.resize(command_buffer_count);
    vk::Result result = device_manager_.device().allocateCommandBuffers(&command_buffer_allocate_info, command_buffers.data());
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to allocate default command buffer", kWho);
    }
}

void Phyre::Graphics::CommandBufferManager::BeginCommandBuffer(vk::CommandBuffer& command_buffer, vk::CommandBufferBeginInfo* info) const {
    if (info) {
        vk::Result result = command_buffer.begin(info);
        if (!ErrorCheck(result, kWho)) {
            Logging::warning("Could not begin the provided command buffer", kWho);
        }
        return;
    }

    // Only used when the command buffer is a secondary command buffer
    vk::CommandBufferInheritanceInfo inheritance_info;
    vk::CommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.setPInheritanceInfo(&inheritance_info);
    vk::Result result = command_buffer.begin(&command_buffer_begin_info);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to begin command buffer", kWho);
    }
}

void Phyre::Graphics::CommandBufferManager::EndCommandBuffer(vk::CommandBuffer& command_buffer) {
    command_buffer.end();
}

void Phyre::Graphics::CommandBufferManager::SubmitToQueue(const vk::Queue& queue, const vk::CommandBuffer& command_buffer, const vk::SubmitInfo* info, const vk::Fence& fence) const {
    uint32_t command_buffer_count = 1;
    if (info) {
        queue.submit(command_buffer_count, info, fence);
        queue.waitIdle();
        return;
    }

    vk::SubmitInfo submit_info;
    submit_info.setCommandBufferCount(command_buffer_count);
    submit_info.setPCommandBuffers(&command_buffer);

    vk::Result result = queue.submit(command_buffer_count, &submit_info, fence);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to submit command buffer to queue", kWho);
    }
}
