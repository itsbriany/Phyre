#include "uniform_buffer.h"
#include "Logging/logging.h"

namespace Phyre {
namespace Graphics {

const std::string UniformBuffer::kWho = "[Graphics::UniformBuffer]";

UniformBuffer::UniformBuffer(const VulkanDevice& device, const vk::Buffer& buffer, const vk::DeviceMemory& memory, const vk::DescriptorBufferInfo& descriptor) :
    device_(device),
    buffer_(buffer),
    memory_(memory),
    descriptor_(descriptor) 
{
    PHYRE_LOG(trace, kWho) << "Instantiated";
}

UniformBuffer::~UniformBuffer() {
    device_.get().destroyBuffer(buffer_);
    device_.get().freeMemory(memory_);
    PHYRE_LOG(trace, kWho) << "Destroyed";
}

}
}
