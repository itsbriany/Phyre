#pragma once

#include <string>

namespace Phyre {
namespace Configuration {

class ResourceManager {
    public:
        ResourceManager(const std::string& phyre_config);

    private:
        std::string phyre_config_;
        
};

}
}