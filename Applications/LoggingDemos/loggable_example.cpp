#include <Logging/logging.h>

using namespace Phyre::Logging;

class Foo : public LoggableInterface {
    public:
        Foo() {
            std::string message = "I am alive!";
            info(message, *this);
        }
        std::string log() override {
            return "[Foo] ";
        }
};

int main()
{
    set_log_level();
    Foo foo;
    return 0;
}
