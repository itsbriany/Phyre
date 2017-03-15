#include <Configuration/configuration_loader.h>

using Phyre::Configuration::ConfigurationLoader;

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        exit(EXIT_FAILURE);
    }
    std::string phyre_config = argv[1];
    ConfigurationLoader cg;
    return 0;
}