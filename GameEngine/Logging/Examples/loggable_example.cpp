#include "logging.h"

using namespace GameEngine::Logging;

class Foo : public Loggable {
    public:
        Foo() {
            std::string message = "I am alive!";
            info(message, *this);
        }
        std::string log() override {
            return "[Foo] ";
        }
};

int main(int argc, char const* argv[])
{
    set_log_level();
    Foo foo;
    return 0;
}
