#include <Configuration/resource_manager.h>

using Phyre::Configuration::ResourceManager;

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        exit(EXIT_FAILURE);
    }
    std::string phyre_config = argv[1];
    ResourceManager rm(phyre_config);
    return 0;
}