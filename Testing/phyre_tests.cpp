#include <gtest/gtest.h>
#include <Networking/networking_tests.h>
#include <XMPP/xmpp_tests.h>
#include <Logging/logging.h>

int main(int argc, char* argv[]) {
    Phyre::Logging::set_log_level(Phyre::Logging::kError);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}