#pragma once
#include <gtest/gtest.h>
#include "vulkan_rendering_system.h"

namespace Phyre {
namespace Graphics {

// TODO: Perhaps instead of crashing, throw exception?
TEST(VulkanRenderingSystemTest, Instantiates) {
    VulkanRenderingSystem rendering_system_;
    EXPECT_TRUE(true);
}

}
}
