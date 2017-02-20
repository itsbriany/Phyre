#pragma once
#include <gtest/gtest.h>
#include "vulkan_rendering_system.h"

namespace Phyre {
namespace Graphics {

TEST(VulkanRenderingSystemTest, Instantiates) {
    VulkanRenderingSystem rendering_system_;
    rendering_system_.Start();
}

}
}
