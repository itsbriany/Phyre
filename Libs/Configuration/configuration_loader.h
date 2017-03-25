#pragma once
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4715) // Ignore Boost warnings
#endif
#include <boost/property_tree/json_parser.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#else
#include <boost/property_tree/json_parser.hpp>
#endif
#include "application.h"
#include <unordered_map>

namespace Phyre {
namespace Configuration {

    class ConfigurationLoader {
    public:
        //----------------- Interface ---------------------
        // Returns true if the phyre config file was properly loaded into memory
        bool LoadConfigurationFromFile(const std::string& path_to_phyre_config) const;

        // Returns true if the phyre config file contents were properly loaded into memory
        bool LoadConfigurationFromContents(const std::string& file_contents) const;

        // Return a pointer to the application. The lifetime is expected to be managed
        // by the caller.
        Application* LoadApplication(const std::string& application_name) const;

    private:
        //----------------- Type Definitions ---------------------
        typedef std::unordered_map<std::string, Application> ApplicationMap;

        //----------------- Helpers ---------------------
        bool ParseConfiguration(boost::property_tree::ptree& pt) const;
        static void ReadJson(const std::string& contents, boost::property_tree::ptree& pt);
        static void ReadJson(std::stringstream& contents, boost::property_tree::ptree& pt);

        //----------------- Data Members ----------------
        std::string phyre_config_;
        std::string file_contents_;
        ApplicationMap application_map_;
        static const std::string kWho;
    };

}
}