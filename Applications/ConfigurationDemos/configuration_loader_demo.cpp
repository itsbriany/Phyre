#include <Configuration/configuration_loader.h>
#include <Configuration/application.h>
#include <Logging/logging.h>
#include <list>

class ConfigurationLoaderDemo {
public:
    ConfigurationLoaderDemo();
    void Run(const std::string& resource_name) const;

private:
    Phyre::Configuration::ConfigurationLoader configuration_loader_;
    std::unique_ptr<Phyre::Configuration::Application> p_application_;
    static const std::string kWho;
};

const std::string ConfigurationLoaderDemo::kWho = "[ConfigurationLoaderDemo]";
ConfigurationLoaderDemo::ConfigurationLoaderDemo() :
    configuration_loader_(),
    p_application_(nullptr) 
{
    p_application_.reset(configuration_loader_.LoadApplication("ConfigurationLoaderDemo"));
    if (!p_application_) {
        PHYRE_LOG(fatal, kWho) << "Failed to load application... Exiting.";
        exit(EXIT_FAILURE);
    }
}

void ConfigurationLoaderDemo::Run(const std::string& resource_name) const
{
    std::ifstream resource = p_application_->ReadResource(resource_name);
    if (resource) {
        PHYRE_LOG(info, kWho) << resource.rdbuf();
        resource.close();
    }
    else {
        PHYRE_LOG(error, kWho) << "Resource not available: " << resource_name;
    }
}

int main()
{
    ConfigurationLoaderDemo demo;
    std::list<std::string> resource_names = { "Example.txt", "Example2.txt", "NotAResource" };
    for (const std::string& resource : resource_names) {
        demo.Run(resource);
    }
    return 0;
}
