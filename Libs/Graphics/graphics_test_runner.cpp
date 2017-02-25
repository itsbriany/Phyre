#include <gtest/gtest.h>
#include "graphics_tests.h"
#include "logging.h"

int main(int argc, char* argv[]) {
    Phyre::Logging::set_log_level(Phyre::Logging::kTrace);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}