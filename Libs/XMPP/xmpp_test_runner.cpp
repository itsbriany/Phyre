#include <gtest/gtest.h>
#include <Logging/logging.h>
#include "xmpp_tests.h"

int main(int argc, char* argv[]) {
    Phyre::Logging::set_log_level();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}