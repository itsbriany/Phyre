#include <Logging/logging.h>
#include "test_loader.h"

Phyre::Utils::TestLoader::Pointer Phyre::Utils::TestLoader::gpTestLoader = nullptr;
const std::string Phyre::Utils::TestLoader::kWho = "[Utils::TestLoader]";

void Phyre::Utils::TestLoader::Initialize(int argc, char* argv[]) {
    if (argc < 2) {
        PHYRE_LOG(fatal, kWho) << "Please specify a phyre config file!";
        exit(EXIT_FAILURE);
    }
    std::string phyre_configuration_path = argv[1];
    gpTestLoader = Pointer(new TestLoader(phyre_configuration_path));
}

Phyre::Utils::TestLoader::Pointer Phyre::Utils::TestLoader::GetInstance() {
    if (!gpTestLoader) {
        PHYRE_LOG(error, kWho) << "Test loader instance has not been initialized!";
    }
    return gpTestLoader;
}

Phyre::Configuration::Provider::Pointer Phyre::Utils::TestLoader::Provide() const {
    return p_configuration_provider_;
}

Phyre::Utils::TestLoader::TestLoader(const std::string& phyre_configuration_path) {
    p_configuration_provider_ = std::make_shared<Configuration::Provider>(phyre_configuration_path);
}