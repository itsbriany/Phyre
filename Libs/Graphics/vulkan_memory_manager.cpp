#include "vulkan_memory_manager.h"
#include "vulkan_errors.h"
#include "vulkan_gpu.h"
#include "vulkan_uniform_buffer.h"

const std::string Phyre::Graphics::VulkanMemoryManager::kWho = "[VulkanMemoryManager]";

Phyre::Graphics::VulkanMemoryManager::VulkanMemoryManager(const VulkanGPU& gpu, const vk::Device& device) : 
    gpu_(gpu),
    device_(device),
    uniform_buffer_(AllocateUniformBuffer()) {
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::VulkanMemoryManager::~VulkanMemoryManager() {
    Logging::trace("Destroyed", kWho);
}

Phyre::Graphics::VulkanUniformBuffer Phyre::Graphics::VulkanMemoryManager::AllocateUniformBuffer() const {
    VulkanUniformBuffer uniform_buffer(device_);
    vk::MemoryRequirements memory_requirements = device_.getBufferMemoryRequirements(uniform_buffer.buffer());
    
    vk::MemoryAllocateInfo allocate_info;
    MemoryPropertyFlags requirements_mask(vk::MemoryPropertyFlagBits::eHostVisible |  // Memory is accessible to host
                                          vk::MemoryPropertyFlagBits::eHostCoherent); // Writes to this memory are visible to both host and device
    allocate_info.setAllocationSize(memory_requirements.size);
    if (!CanFindMemoryTypeFromProperties(memory_requirements.memoryTypeBits, requirements_mask, allocate_info.memoryTypeIndex)) {
        Logging::fatal("Failed to satisfy physical device memory requirements for allocating buffer", kWho);
    }

    vk::DeviceMemory device_memory;
    vk::Result result = device_.allocateMemory(&allocate_info, nullptr, &device_memory);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to allocate memory for buffer", kWho);
    }
    
    // Temporarily give the CPU access to this memory
    vk::MemoryMapFlags flags;
    void* p_data;
    result = device_.mapMemory(device_memory, 0, memory_requirements.size, flags, static_cast<void**>(&p_data));
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to map data to memory on physical device", kWho);
    }

    memcpy(p_data, &uniform_buffer.mvp(), sizeof(uniform_buffer.mvp()));
    
    // Immediately unmap the memory because the page tables are limited in size for memory visible to
    // both CPU and GPU
    device_.unmapMemory(device_memory);
    device_.bindBufferMemory(uniform_buffer.buffer(), device_memory, 0);
    uniform_buffer.set_memory(device_memory);
    return uniform_buffer;
}

bool Phyre::Graphics::VulkanMemoryManager::CanFindMemoryTypeFromProperties(uint32_t memory_type_bits,
                                                                           MemoryPropertyFlags requirements_mask,
                                                                           uint32_t& memory_type_index) const {
    const vk::PhysicalDeviceMemoryProperties& memory_properties = gpu_.memory_properties();
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
        if ((memory_type_bits & uint32_t(vk::MemoryPropertyFlagBits::eDeviceLocal)) == 1) {
            // Type is available, does it match user properties?
            if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                memory_type_index = i;
                return true;
            }
        }
        memory_type_bits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}
