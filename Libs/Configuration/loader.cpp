#include <vector>
#include <Logging/logging.h>
#include "loader.h"
#include <gtest/internal/gtest-port.h>

const std::string Phyre::Configuration::Loader::kWho = "[Configuration::Loader]";

Phyre::Configuration::Loader::Loader(const std::string& phyre_config_path) :
    target_("phyre.json"),
    phyre_config_path_(phyre_config_path),
    phyre_config_directory_(phyre_config_path)
{
    size_t replace_pos = phyre_config_directory_.find(target_);
    if (replace_pos != std::string::npos) {
        phyre_config_directory_.replace(replace_pos, target_.size(), "");
    }
    LoadConfigurationFromFile(phyre_config_path_);
}

bool Phyre::Configuration::Loader::LoadConfigurationFromFile(const std::string& path_to_phyre_config) {
    boost::property_tree::ptree root;
    boost::property_tree::read_json(path_to_phyre_config, root);
    return ParseConfiguration(root);
}

Phyre::Configuration::Application* Phyre::Configuration::Loader::LoadApplication(const std::string& application_name) const {
    ApplicationMap::const_iterator cit = application_map_.find(application_name);
    ApplicationMap::const_iterator cend = application_map_.cend();
    if (cit == cend) {
        return nullptr;
    }
    return new Application(cit->second);
}

std::string Phyre::Configuration::Loader::GetContents(const std::string& target_application, const std::string& resource) const {
    std::unique_ptr<Application> p_application(LoadApplication(target_application));
    if (p_application) {
        std::ifstream resource_stream = p_application->OpenResource(resource);
        std::ostringstream oss;
        oss << resource_stream.rdbuf();
        std::string result = oss.str();
        resource_stream.close();
        return result;
    }
    return std::string();
}

bool Phyre::Configuration::Loader::ParseConfiguration(boost::property_tree::ptree& pt) {
    for (const boost::property_tree::ptree::value_type& value : pt.get_child("Phyre.Applications")) {
        for (const boost::property_tree::ptree::value_type& application : value.second) {
            const std::string &name(application.first);
            std::pair<std::string, Application> entry = std::make_pair(name, ApplicationFromPtree(application));
            application_map_.insert(entry);
        }
    }
    return true;
}

Phyre::Configuration::Application Phyre::Configuration::Loader::ApplicationFromPtree(const boost::property_tree::ptree::value_type& node) const {
    std::ostringstream full_path;
    full_path << phyre_config_directory_ << node.second.get<std::string>("Path");
    Application::ResourceSet resources;
    for (const boost::property_tree::ptree::value_type& resource : node.second.get_child("Resources")) {
        const std::string &resource_name = resource.second.get_value<std::string>(""); // Default to the empty string
        resources.insert(resource_name);
    }
    return Application(full_path.str(), resources);
}
