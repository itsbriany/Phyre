#include <gtest/gtest.h>
#include <Logging/logging.h>
#include <Configuration/Test/configuration_tests.h>
#include <Networking/Test/networking_tests.h>
#include <XMPP/Test/xmpp_tests.h>
#include <Utils/test_loader.h>

int main(int argc, char* argv[]) {
    using Phyre::Utils::TestLoader;
    using Phyre::Logging::set_log_level;
    
    set_log_level(Phyre::Logging::kTrace);
    testing::InitGoogleTest(&argc, argv);
 
    TestLoader::Initialize(argc, argv);
    return RUN_ALL_TESTS();
}
