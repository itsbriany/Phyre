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
    ASSERT_FALSE(contents.empty());
    EXPECT_STREQ("Hello world", contents.c_str());
}

TEST_F(ConfigurationLoaderTest, GetContentsNoSuchResource) {
    std::string resource("no_such_resource.res");
    std::string contents = p_provider_->GetContents(target_, resource);
    EXPECT_TRUE(contents.empty());
}

TEST_F(ConfigurationLoaderTest, GetContentsBinary) {
    std::string resource("vertices.spv");
    std::string contents = p_provider_->GetContents(target_, resource);
    ASSERT_FALSE(contents.empty());
    EXPECT_EQ(996u, contents.size());
    EXPECT_EQ(contents[0], 0x03);
    EXPECT_EQ(contents[1], 0x02);
    EXPECT_EQ(contents[2], 0x23);
    EXPECT_EQ(contents[3], 0x07);
}

TEST_F(ConfigurationLoaderTest, GetContentsUint32t) {
    std::string resource("vertices.spv");
    std::vector<uint32_t> contents = p_provider_->GetContentsUint32t(target_, resource);
    ASSERT_FALSE(contents.empty());
    EXPECT_EQ(996u, contents.size() * sizeof(uint32_t));
    EXPECT_EQ(contents[0], 0x07230203u);
}

TEST_F(ConfigurationLoaderTest, GetContentsUint32tNoSuchResource) {
    std::string resource("novertices.spv");
    std::vector<uint32_t> contents = p_provider_->GetContentsUint32t(target_, resource);
    ASSERT_TRUE(contents.empty());
}

TEST_F(ConfigurationLoaderTest, GetContentsSPIRV) {
    std::string resource("vertices.spv");
    std::vector<uint32_t> contents = p_provider_->GetContentsSPIRV(target_, resource);
    ASSERT_FALSE(contents.empty());
    EXPECT_EQ(996u, contents.size() * sizeof(uint32_t));
    EXPECT_EQ(contents[0], 0x07230203u);
}
