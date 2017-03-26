#pragma once
#include <string>
#include <unordered_set>

namespace Phyre {
namespace Configuration {

/*
 * Represents the strucuture from phyre.json in memory
 */
class Configuration {
public:
    // ----------------- Type Definitions -------------------
    typedef std::unordered_set<std::string> ApplicationSet;

    //------------------ Construction -----------------------
    Configuration(const ApplicationSet& application_set);

    //--------------------- Interface -----------------------
    const ApplicationSet& applications() const { return application_set_; }

private:
    // The set of applications which can be ran within Phyre
    ApplicationSet application_set_;
};

}
}
