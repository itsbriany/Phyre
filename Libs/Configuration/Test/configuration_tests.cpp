#include <gtest/gtest.h>
#include <Configuration/loader.h>
#include "configuration_tests.h"

namespace {
    std::string gPathToPhyreConfig;
}

class ConfigurationLoaderTest : public ::testing::Test {
protected:
    //-------------------- Setup & TearDown --------------------
    ConfigurationLoaderTest() : target_("ConfigurationLoaderTest") {
        p_loader_.reset(new Phyre::Configuration::Loader(gPathToPhyreConfig));
    }

    virtual ~ConfigurationLoaderTest() {}

    //-------------------- Data Members ------------------------
    std::string target_;
    std::unique_ptr<Phyre::Configuration::Loader> p_loader_;
};

TEST_F(ConfigurationLoaderTest, Initialize) {
    std::unique_ptr<Phyre::Configuration::Application> p_app(p_loader_->LoadApplication(target_));
    ASSERT_TRUE(p_app);
}

TEST_F(ConfigurationLoaderTest, GetContents) {
    std::string resource("Example.txt");
    std::string contents = p_loader_->GetContents(target_, resource);
    EXPECT_FALSE(contents.empty());
#if defined(_MSC_VER)
    EXPECT_STREQ("Hello\r\nworld", contents.c_str());
#else
    EXPECT_STREQ("Hello\nworld", contents.c_str());
#endif
}

TEST_F(ConfigurationLoaderTest, GetContentsNoSuchResource) {
    std::string resource("no_such_resource.res");
    std::string contents = p_loader_->GetContents(target_, resource);
    EXPECT_TRUE(contents.empty());
}

void Phyre::Configuration::Tests::SetConfigurationFilePath(const std::string& phyre_configuration_path) {
    gPathToPhyreConfig = phyre_configuration_path;
}
