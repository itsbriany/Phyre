#pragma once
#include <gtest/gtest.h>
#include "vulkan_rendering_system.h"

namespace Phyre {
namespace Graphics {

class VulkanRenderingSystemTest : public testing::Test {
protected:
    // Prevents resource leak
    virtual ~VulkanRenderingSystemTest() { }

    // Test members
    VulkanRenderingSystem  rendering_system_;
};

TEST_F(VulkanRenderingSystemTest, CanInstantiate) {
    EXPECT_TRUE(rendering_system_.InitializeVulkanInstance());
}

TEST_F(VulkanRenderingSystemTest, CanEnumerateDevices) {
    EXPECT_TRUE(rendering_system_.InitializePhysicalDevices());
}

TEST_F(VulkanRenderingSystemTest, SupportsGraphics) {
    EXPECT_TRUE(rendering_system_.CheckGraphicsCapability().first);
}

TEST_F(VulkanRenderingSystemTest, InitializesLogicalDevice) {
    EXPECT_TRUE(rendering_system_.InitializeLogicalDevice());
}

}
}
