#pragma once
#include <fstream>
#include <set>
#include <string>

namespace Phyre {
namespace Configuration {

class Application {
public:
    Application(const std::string& path, const std::set<std::string>& resource_set);
    std::ifstream ReadResource(const std::string& resource_name) const;

private:
    std::string path_;
    std::set<std::string> resource_set_;
};

}
}
