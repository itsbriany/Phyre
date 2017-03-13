#include <Logging/logging.h>

using namespace Phyre::Logging;

class Foo {
    public:
        Foo() {
            std::string message = "I am alive!";
            PHYRE_LOG(trace, kWho) << message;
            PHYRE_LOG(info, kWho) << message;
            PHYRE_LOG(warning, kWho) << message;
            PHYRE_LOG(error, kWho) << message;
            PHYRE_LOG(fatal, kWho) << message;
        }
    
    const std::string kWho = "[Foo]";
};



int main()
{
    set_log_level();
    Foo foo;
    return 0;
}
