#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include "logging.h"
#include "tcp_server.h"
#include "xmpp_client.h"

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

        Sked read(std::istream& ss) const
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

class XMPPClientTest : public ::testing::Test, public Logging::Loggable {
    protected:
        XMPPClientTest():
            host_("127.0.0.1"),
            port_or_service_("1234"),
            tcp_server_(TCPServer(io_service_, std::stoi(port_or_service_))) { }

        virtual ~XMPPClientTest() { }

        static std::string resource_directory() { 
            return "XMPPTestResources/";
        }
       
        std::string authentication_mechanism_response() {
            return read_file(resource_directory() + "authentication_selection.xml");
        }

        std::string authentication_mechanism_response_first_chunk() {
            return read_file(resource_directory() + "authentication_selection_chunk1.xml");
        }

        std::string authentication_mechanism_response_second_chunk() {
            return read_file(resource_directory() + "authentication_selection_chunk2.xml");
        }

        // Reads a file without a newline at the end
        std::string read_file(const std::string& file_name) {
            std::ifstream input(file_name);
            std::ostringstream oss;
            if (!input) {
                Logging::warning("Could not read file!", *this);
                return "";
            }

            oss << input.rdbuf();
            input.close();

            std::string response = oss.str();
            response.pop_back();
            return response;
        }

        void SetUp() override {
            p_xmpp_client_ = std::make_unique<XMPPClient>(io_service_);
        }

        std::string log() override {
            return "[XMPPClientTest]";
        }

        boost::asio::io_service io_service_;
        std::string host_;
        std::string port_or_service_;
        TCPServer tcp_server_;
        std::unique_ptr<XMPPClient> p_xmpp_client_;

};

TEST_F(XMPPClientTest, ExtractAuthenticationMechanismResponseExcessData) {
    std::string response = authentication_mechanism_response();
    std::string expected = response;

    std::string excess = "More data";
    std::string bytes_read = response + excess;

    std::stringstream extracted_response = p_xmpp_client_->ExtractAuthenticationMechanismResponse(bytes_read);
    EXPECT_STREQ(extracted_response.str().c_str(), expected.c_str());
    EXPECT_STREQ(p_xmpp_client_->buffer().c_str(), excess.c_str());
}

TEST_F(XMPPClientTest, ExtractAuthenticationMechanismResponseChunks) {
    std::string first_chunk = authentication_mechanism_response_first_chunk();
    std::string second_chunk = authentication_mechanism_response_second_chunk();

    std::stringstream extracted_response = p_xmpp_client_->ExtractAuthenticationMechanismResponse(first_chunk);
    EXPECT_TRUE(extracted_response.str().empty());
    EXPECT_STREQ(p_xmpp_client_->buffer().c_str(), first_chunk.c_str());

    extracted_response = p_xmpp_client_->ExtractAuthenticationMechanismResponse(second_chunk);
    std::string expected_response = first_chunk + second_chunk;
    EXPECT_TRUE(p_xmpp_client_->buffer().empty());
    EXPECT_STREQ(extracted_response.str().c_str(), expected_response.c_str());
}

TEST_F(XMPPClientTest, ParsesAuthenticationMechanisms) {
    std::string payload = authentication_mechanism_response();
    std::stringstream ss;
    ss << payload;

    std::string plain = "PLAIN";
    std::string md5 = "DIGEST-MD5";
    std::string oauth = "X-OAUTH2";
    std::string sha1 = "SCRAM-SHA-1";
    std::unordered_set<std::string> authentication_mechanism_set = p_xmpp_client_->ParseAuthenticationMechanisms(ss);
    EXPECT_TRUE(authentication_mechanism_set.find(plain) != authentication_mechanism_set.end());
    EXPECT_TRUE(authentication_mechanism_set.find(md5) != authentication_mechanism_set.end());
    EXPECT_TRUE(authentication_mechanism_set.find(oauth) != authentication_mechanism_set.end());
    EXPECT_TRUE(authentication_mechanism_set.find(sha1) != authentication_mechanism_set.end());
}

TEST_F(XMPPClientTest, HandlesAuthenticationMeachanismSelectionState) {
    std::string payload = authentication_mechanism_response();
    EXPECT_EQ(p_xmpp_client_->state(), XMPPClient::TransactionState::kSelectAuthenticationMechanism);

    p_xmpp_client_->HandleSelectAuthenticationMechanism(payload);
    EXPECT_EQ(p_xmpp_client_->state(), XMPPClient::TransactionState::kDecodeBase64Challenge);
}

}
}