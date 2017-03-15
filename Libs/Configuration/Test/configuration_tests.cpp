#include <gtest/gtest.h>
#include <Logging/logging.h>

class ConfigurationManagerTest : public ::testing::Test {
protected:
    ConfigurationManagerTest() { }

    virtual ~ConfigurationManagerTest() {}

};

TEST_F(ConfigurationManagerTest, Works) {
    std::cout << "bar" << std::endl;
    EXPECT_TRUE(true);
}

int main(int argc, char* argv[]) {
    Phyre::Logging::set_log_level();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}