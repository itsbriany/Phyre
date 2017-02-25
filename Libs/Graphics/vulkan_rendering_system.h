#pragma once
#include "rendering_system_interface.h"
#include "vulkan_loader.h"

namespace Phyre {
namespace Graphics {

class VulkanRenderingSystem : public RenderingSystemInterface {
public:
    VulkanRenderingSystem();

    // TODO This can probably be used for global cleanup
    virtual ~VulkanRenderingSystem();

    // Returns true if the rendering system started correctly
    void Start() override;

private:
    // Loads the system and used to access various components
    VulkanLoader loader_;

    static const std::string kWho;
};

}
}
