#pragma once

#include <gtest/gtest.h>
#include <Configuration/provider.h>
#include <Utils/test_loader.h>

class ConfigurationLoaderTest : public ::testing::Test {
protected:
    //-------------------- Setup & TearDown --------------------
    ConfigurationLoaderTest() : target_("ConfigurationLoaderTest") {
        p_provider_ = Phyre::Utils::TestLoader::GetInstance()->Provide();
    }

    virtual ~ConfigurationLoaderTest() {}

    //-------------------- Data Members ------------------------
    std::string target_;
    Phyre::Configuration::Provider::Pointer p_provider_;
};

TEST_F(ConfigurationLoaderTest, Initialize) {
    std::unique_ptr<Phyre::Configuration::Application> p_app(p_provider_->LoadApplication(target_));
    ASSERT_TRUE(p_app);
}

TEST_F(ConfigurationLoaderTest, GetContents) {
    std::string resource("Example.txt");
    std::string contents = p_provider_->GetContents(target_, resource);
    EXPECT_FALSE(contents.empty());
    EXPECT_STREQ("Hello world", contents.c_str());
}

TEST_F(ConfigurationLoaderTest, GetContentsNoSuchResource) {
    std::string resource("no_such_resource.res");
    std::string contents = p_provider_->GetContents(target_, resource);
    EXPECT_TRUE(contents.empty());
}