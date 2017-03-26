#include <gtest/gtest.h>
#include <Configuration/loader.h>
#include <Logging/logging.h>

// We need to somehow access the command line arguments
std::string path_to_phyre_config;

class ConfigurationLoaderTest : public ::testing::Test {
protected:
    ConfigurationLoaderTest() : target_("ConfigurationLoaderTest") {
        p_loader_.reset(new Phyre::Configuration::Loader(path_to_phyre_config));
    }

    virtual ~ConfigurationLoaderTest() {}

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
    EXPECT_STREQ("Hello\r\nworld", contents.c_str());
}

int main(int argc, char* argv[]) {
    Phyre::Logging::set_log_level(Phyre::Logging::kTrace);
    testing::InitGoogleTest(&argc, argv);
    if (argc < 2) {
        PHYRE_LOG(fatal, "[ConfigurationTests]") << "Please specify a phyre config file!";
        exit(EXIT_FAILURE);
    }
    path_to_phyre_config = argv[1];
    return RUN_ALL_TESTS();
}