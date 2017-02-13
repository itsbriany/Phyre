#pragma once
#include <gtest/gtest.h>
#include "rendering_system.h"

namespace Phyre {
namespace Graphics {

class RenderingSystemTest : public testing::Test {
protected:
    // Prevents resource leak
    virtual ~RenderingSystemTest() { }

    // Test members
    RenderingSystem  rendering_system_;
};

TEST_F(RenderingSystemTest, CanInstantiate) {
    EXPECT_TRUE(RenderingSystem::CreateVulkanInstance());
}

TEST_F(RenderingSystemTest, CanEnumerateDevices) {
    RenderingSystem::PhysicalDeviceVector devices = rendering_system_.EnumeratePhysicalDevices();
    size_t expected_device_count = 1;
    EXPECT_GE(devices.size(), expected_device_count);
}

TEST_F(RenderingSystemTest, SupportsGraphics) {
    EXPECT_GT(rendering_system_.CheckGraphicsCapability().queueCount, 0);
}

TEST_F(RenderingSystemTest, CreatesLogicalDevice) {
    EXPECT_TRUE(rendering_system_.CreateDevice());
}

}
}
