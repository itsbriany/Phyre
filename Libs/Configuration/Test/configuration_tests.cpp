#include <Logging/logging.h>
#include "configuration_tests.h"

int main(int argc, char* argv[]) {
    using Phyre::Utils::TestLoader;
    using Phyre::Logging::set_log_level;

    set_log_level(Phyre::Logging::kTrace);
    testing::InitGoogleTest(&argc, argv);

    TestLoader::Initialize(argc, argv);
    return RUN_ALL_TESTS();
}
