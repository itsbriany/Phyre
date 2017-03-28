#pragma once
#include <memory>
#include <Configuration/provider.h>

namespace Phyre {
namespace Utils {

class TestLoader {

public:
    //----------------------- Type Definitions -----------------

    typedef std::shared_ptr<TestLoader> Pointer;

    //----------------------- Interface ------------------------

    /**
     * \param argc The argument count from the command line
     * \param argv The mutable command line arguments
     * \brief Initialize the singleton instance
     */
    static void Initialize(int argc, char* argv[]);

    /**
    * \return Access the underlying singleton instance
    */
    static Pointer GetInstance();

    /**
    * \return A pointer to the configuration provider
    */
    Configuration::Provider::Pointer Provide() const;

private:
    //----------------------- Construction ---------------------

    /**
    * \param path_to_phyre_config The absolute path to phyre.json
    */
    TestLoader(const std::string& path_to_phyre_config);

    //----------------------- Data Members ----------------------

    // Used to access phyre.json
    Configuration::Provider::Pointer p_configuration_provider_;

    //----------------------- Globals ---------------------------

    // The underlying test loader singleton
    static Pointer gpTestLoader;

    //----------------------- Logging ---------------------------

    static const std::string kWho;
};

}
}