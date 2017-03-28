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

 /**
  * \brief Interface to access the data from the phyre.json configuration structure
  */
class Provider {
public:
    //----------------- Type Definitions --------------

    typedef std::shared_ptr<Provider> Pointer;

    //----------------- Construction ------------------

    /**
     * \param phyre_config_path The absolute path to phyre.json 
     */
    explicit Provider(const std::string& phyre_config_path);

    //----------------- Interface ---------------------

    /**
     * \brief The lifetime is expected to be managed by the caller.
     * \param application_name 
     * \return Pointer to the loaded application on success. 
     * \return nullptr on failure.
     */
    Application* LoadApplication(const std::string& application_name) const;

    /**
     * \param target_application the application from which we are loading the resource data from
     * \param resource the file we wish to open
     * \return The contents of the resource. The contents will be empty if there is no resource.
     */
    std::string GetContents(const std::string& target_application, const std::string& resource) const;

private:
    //----------------- Type Definitions ---------------------
   
    /**
     * \brief Map applications by their name
     */
    typedef std::unordered_map<std::string, Application> ApplicationMap;

    //--------------------- Helpers --------------------------

    /**
     * \param path_to_phyre_config The file we wish to load the configuration from
     * \return true if the phyre config file was properly loaded into memory
     */
    bool LoadConfigurationFromFile(const std::string& path_to_phyre_config);

    /**
     * \param root The root of the property tree
     * \return true if the configuration was properly parsed
     */
    bool ParseConfiguration(boost::property_tree::ptree& root);

    Application ApplicationFromPtree(const boost::property_tree::ptree::value_type& node) const;

    //----------------- Data Members ----------------
    
    // The name of the target phyre.json
    std::string target_;

    // The absolute path the phyre.json file
    std::string phyre_config_path_;

    // The absolute path to the directory containing phyre.json
    std::string phyre_config_directory_;

    // Represents the following mapping (Application Name -> Application Data)
    ApplicationMap application_map_;

    //----------------- Logging Helper---------------
    static const std::string kWho;
};

}
}