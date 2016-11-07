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

        std::string generic_xml() {
            return read_file(resource_directory() + "generic.xml");
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

        std::string base64_challenge() {
            return read_file(resource_directory() + "base64_challenge.xml");
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

TEST_F(XMPPClientTest, ExtractXML) {
    std::string payload = generic_xml();
    std::string start_tag = "<stream:features>";
    std::string end_tag = "</stream:features>";
    size_t start_tag_pos = payload.find(start_tag);
    size_t end_tag_pos = payload.find(end_tag);

    p_xmpp_client_->buffer() += payload;
    std::string extracted_xml = p_xmpp_client_->ExtractXML(start_tag, end_tag).str();
    std::string expected_xml = payload.substr(start_tag_pos, end_tag_pos + end_tag.size() - start_tag_pos);
    EXPECT_STREQ(extracted_xml.c_str(), expected_xml.c_str());

    std::string remaining_bytes = payload.substr(end_tag_pos + end_tag.size());
    EXPECT_STREQ(p_xmpp_client_->buffer().c_str(), remaining_bytes.c_str());
}

TEST_F(XMPPClientTest, ExtractAuthenticationMechanismResponseExcessData) {
    std::string response = authentication_mechanism_response();
    std::string expected = response.substr(response.find("<stream:features>"));

    std::string excess = "More data";
    std::string bytes_read = response + excess;

    p_xmpp_client_->buffer() += bytes_read;
    std::stringstream extracted_response = p_xmpp_client_->ExtractAuthenticationMechanismResponse(bytes_read);
    EXPECT_STREQ(extracted_response.str().c_str(), expected.c_str());
    EXPECT_STREQ(p_xmpp_client_->buffer().c_str(), excess.c_str());
}

TEST_F(XMPPClientTest, ExtractAuthenticationMechanismResponseChunks) {
    std::string first_chunk = authentication_mechanism_response_first_chunk();
    std::string second_chunk = authentication_mechanism_response_second_chunk();

    p_xmpp_client_->buffer() += first_chunk;
    std::stringstream extracted_response = p_xmpp_client_->ExtractAuthenticationMechanismResponse(first_chunk);
    EXPECT_TRUE(extracted_response.str().empty());

    p_xmpp_client_->buffer() += second_chunk;
    extracted_response = p_xmpp_client_->ExtractAuthenticationMechanismResponse(second_chunk);
    std::string expected_response = first_chunk + second_chunk;
    EXPECT_STREQ(extracted_response.str().c_str(), expected_response.c_str());
}

TEST_F(XMPPClientTest, ParsesAuthenticationMechanisms) {
    std::string response = authentication_mechanism_response();
    std::string payload = response.substr(response.find("<stream:features>"));
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

    p_xmpp_client_->OnRead(payload);
    EXPECT_EQ(p_xmpp_client_->state(), XMPPClient::TransactionState::kDecodeBase64Challenge);
}

TEST_F(XMPPClientTest, ParsesBase64Challenge) {
    std::string expected = "bm9uY2U9IjM0ODE5ODkxNDYyNDY2NTcyMjQiLHFvcD0iYXV0aCIsY2hhcnNldD11dGYtOCxhbGdvcml0aG09bWQ1LXNlc3M=";
    std::stringstream ss;
    ss << base64_challenge();

    std::string parsed_challenge = p_xmpp_client_->ParseBase64Challenge(ss);
    EXPECT_STREQ(parsed_challenge.c_str(), expected.c_str());
}

TEST_F(XMPPClientTest, DecodesBase64) {
    std::string input = "bm9uY2U9IjM0ODE5ODkxNDYyNDY2NTcyMjQiLHFvcD0iYXV0aCIsY2hhcnNldD11dGYtOCxhbGdvcml0aG09bWQ1LXNlc3M=";
    std::string expected = "nonce=\"3481989146246657224\",qop=\"auth\",charset=utf-8,algorithm=md5-sess";

    std::string decoded = p_xmpp_client_->DecodeBase64(input);
    EXPECT_STREQ(decoded.c_str(), expected.c_str());
}

}
}
