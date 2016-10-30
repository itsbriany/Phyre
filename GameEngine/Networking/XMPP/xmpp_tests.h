#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <gtest/gtest.h>
#include "logging.h"

namespace GameEngine
{
namespace Networking
{
    // Simple examples to learn boost::property_tree
    class XMLParseTest : public ::testing::Test {
        protected:
            XMLParseTest() { }
            virtual ~XMLParseTest() { }

            struct Flight {
                std::string carrier;
                unsigned number;
                bool cancelled;
            };
            typedef std::vector<Flight> Sked;

            Sked read( std::istream & ss )
            {
                // populate tree structure pt
                using boost::property_tree::ptree;
                ptree pt;
                read_xml(ss, pt);

                // traverse pt
                Sked ans;
                for (ptree::value_type const& v : pt.get_child("sked")) {
                    if(v.first == "flight") {
                        Flight f;
                        f.carrier = v.second.get<std::string>("carrier");
                        f.number = v.second.get<unsigned>("number");
                        f.cancelled = v.second.get("<xmlattr>.cancelled", false);
                        ans.push_back(f);
                    }
                }

                return ans;
            }
    };

    // Example from https://akrzemi1.wordpress.com/2011/07/13/parsing-xml-with-boost/
    TEST_F(XMLParseTest, CanParseXML) {
        std::stringstream ss;
        ss << "<?xml version=\"1.0\"?>";
        ss << "<sked>";
        ss << "  <version>2</version>";
        ss << "  <flight>";
        ss << "    <carrier>BA</carrier>";
        ss << "    <number>4001</number>";
        ss << "  </flight>";
        ss << "  <flight cancelled=\"true\">";
        ss << "    <carrier>BA</carrier>";
        ss << "    <number>4002</number>";
        ss << "  </flight>";
        ss << "</sked>";

        Sked sked = read(ss);
        EXPECT_EQ(sked.size(), 2);
        EXPECT_EQ(sked[0].carrier, "BA");
        EXPECT_EQ(sked[0].number, 4001);
        EXPECT_FALSE(sked[0].cancelled);
        EXPECT_EQ(sked[1].carrier, "BA");
        EXPECT_EQ(sked[1].number, 4002);
        EXPECT_TRUE(sked[1].cancelled);
    }

    class XMPPClientTest : public ::testing::Test {
        protected:
            XMPPClientTest() { }
            virtual ~XMPPClientTest() { }
    };

    TEST_F(XMPPClientTest, CanAuthenticateWithServer) {

    }
}
}
