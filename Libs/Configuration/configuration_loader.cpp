#include <fstream>
#include <Logging/logging.h>
#include "configuration_loader.h"

const std::string Phyre::Configuration::ConfigurationLoader::kWho = "[ConfigurationLoader]";

bool Phyre::Configuration::ConfigurationLoader::LoadConfigurationFromFile(const std::string& path_to_phyre_config) const {
    boost::property_tree::ptree pt;
    // read_json(path_to_phyre_config, pt);
    return ParseConfiguration(pt);
}

bool Phyre::Configuration::ConfigurationLoader::LoadConfigurationFromContents(const std::string& file_contents) const {
    std::stringstream ss;
    ss << file_contents;
    boost::property_tree::ptree pt;
    try {
        boost::property_tree::read_json(ss, pt);
        return ParseConfiguration(pt);
    } catch (boost::property_tree::json_parser_error& e) {
        PHYRE_LOG(fatal, kWho) << e.what();
        return false;
    }
}

Phyre::Configuration::Application* Phyre::Configuration::ConfigurationLoader::LoadApplication(const std::string& application_name) const {
    ApplicationMap::const_iterator cit = application_map_.find(application_name);
    ApplicationMap::const_iterator cend = application_map_.cend();
    if (cit != cend) {
        return nullptr;
    }
    return new Application(cit->second);
}

bool Phyre::Configuration::ConfigurationLoader::ParseConfiguration(boost::property_tree::ptree& pt) const {
    for (boost::property_tree::ptree::value_type& value : pt.get_child("Phyre")) {
        PHYRE_LOG(info, kWho) << value.first;
    }
    return true;
 
    
    
    //for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it) {
    //    PHYRE_LOG(info, kWho) << '(' << it->first << ", " << it->second << ')';
    //   /* if (it->first == "mechanism") {
    //        std::string mechanism_value = it->second.get_value<std::string>();
    //        authentication_mechanism_set.insert(mechanism_value);
    //    }
    //    ParseAuthenticationMechanismsRecursive(it->second, authentication_mechanism_set);*/
    //}
}
