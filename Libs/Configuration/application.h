#pragma once
#include <fstream>
#include <set>
#include <string>
#include <unordered_set>

namespace Phyre {
namespace Configuration {

/*
 * Represents a Phyre application in memory
 */
class Application {
public:
    //------------------------ Type Definitions -----------------------
    typedef std::unordered_set<std::string> ResourceSet;

    //------------------------ Construction ---------------------------

    // @param full_path The full path to the application
    // @param resource_set The set of resources this application depends on
    Application(const std::string& full_path, const ResourceSet& resource_set);
    
    //------------------------ Interface ------------------------------

    // @param resource_name The resource we wish to access
    // @return The stream of data from reading the resource
    std::ifstream OpenResource(const std::string& resource_name) const;

private:
    // The path to where this application can be accessed
    std::string path_;

    // The set of resources that this application uses relative to its path
    ResourceSet resource_set_;
};

}
}
