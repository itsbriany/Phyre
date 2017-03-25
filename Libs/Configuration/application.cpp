#include <sstream>
#include "application.h"

Phyre::Configuration::Application::Application(const std::string& path, const std::set<std::string>& resource_set) :
    path_(path),
    resource_set_(resource_set) { }

std::ifstream Phyre::Configuration::Application::ReadResource(const std::string& resource_name) const {
    std::ostringstream full_path;
    full_path << path_ << '/' << resource_name;
    std::ifstream ifs(full_path.str(), std::ifstream::binary);
    return ifs;
}
