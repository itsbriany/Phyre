#include <gtest/gtest.h>
#include "xmpp_tests.h"
#include "logging.h"

int main(int argc, char* argv[]) {
    GameEngine::Logging::set_log_level();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}