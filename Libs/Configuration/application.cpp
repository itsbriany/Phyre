#include <boost/algorithm/string/replace.hpp>
#include <sstream>
#include "application.h"

Phyre::Configuration::Application::Application(const std::string& path, const ResourceSet& resource_set) :
    path_(path),
    resource_set_(resource_set) { }

std::ifstream Phyre::Configuration::Application::OpenResource(const std::string& resource_name) const {
    std::ostringstream full_path;
    full_path << path_ << '/' << resource_name;
    std::string full_path_string = full_path.str();
    
    // The path needs to be readable, so we cannot have forward slashes in our path
    boost::algorithm::replace_all(full_path_string, "\\", "/");
    std::ifstream ifs(full_path_string, std::ifstream::binary);
    return ifs;
}
