#pragma once
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include "logging.h"
#include "sasl.h"
#include "tcp_server.h"
#include "xmpp_client.h"

namespace Phyre
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

class SASLTest : public ::testing::Test, public Logging::LoggableInterface {
    protected:
        SASLTest():
            host_("localhost"),
            port_or_service_("1234"),
            username_("admin"),
            password_("password"),
            tcp_server_(TCPServer(io_service_, boost::lexical_cast<uint16_t>(port_or_service_))) { }

        virtual ~SASLTest() { }

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
                Logging::error("Could not read file!", *this);
                return "";
            }

            oss << input.rdbuf();
            input.close();

            std::string response = oss.str();
            response.pop_back();
            return response;
        }

        void SetUp() override {
            ptr_xmpp_client_ = std::make_unique<XMPPClient>(io_service_, host_, username_, password_);
			ptr_sasl_ = std::make_unique<SASL>(*ptr_xmpp_client_);
        }

        std::string log() override {
            return "[SASLTest]";
        }

        boost::asio::io_service io_service_;
        std::string host_;
        std::string port_or_service_;
        std::string username_;
        std::string password_;
        TCPServer tcp_server_;
        std::unique_ptr<XMPPClient> ptr_xmpp_client_;
        std::unique_ptr<SASL> ptr_sasl_;
};

TEST_F(SASLTest, ExtractXML) {
    std::string payload = generic_xml();
    std::string start_tag = "<stream:features>";
    std::string end_tag = "</stream:features>";
    size_t start_tag_pos = payload.find(start_tag);
    size_t end_tag_pos = payload.find(end_tag);

    ptr_xmpp_client_->buffer() += payload;
    std::string extracted_xml = ptr_sasl_->ExtractXML(start_tag, end_tag).str();
    std::string expected_xml = payload.substr(start_tag_pos, end_tag_pos + end_tag.size() - start_tag_pos);
    EXPECT_STREQ(extracted_xml.c_str(), expected_xml.c_str());

    std::string remaining_bytes = payload.substr(end_tag_pos + end_tag.size());
    EXPECT_STREQ(ptr_xmpp_client_->buffer().c_str(), remaining_bytes.c_str());
}

TEST_F(SASLTest, ExtractAuthenticationMechanismResponseExcessData) {
    std::string response = authentication_mechanism_response();
    std::string expected = response.substr(response.find("<stream:features>"));

    std::string excess = "More data";
    std::string bytes_read = response + excess;

    ptr_xmpp_client_->buffer() += bytes_read;
    std::stringstream extracted_response = ptr_sasl_->ExtractAuthenticationMechanismResponse();
    EXPECT_STREQ(extracted_response.str().c_str(), expected.c_str());
    EXPECT_STREQ(ptr_xmpp_client_->buffer().c_str(), excess.c_str());
}

TEST_F(SASLTest, ExtractAuthenticationMechanismResponseChunks) {
    std::string first_chunk = authentication_mechanism_response_first_chunk();
    std::string second_chunk = authentication_mechanism_response_second_chunk();

    ptr_xmpp_client_->buffer() += first_chunk;
    std::stringstream extracted_response = ptr_sasl_->ExtractAuthenticationMechanismResponse();
    EXPECT_TRUE(extracted_response.str().empty());

    ptr_xmpp_client_->buffer() += second_chunk;
    extracted_response = ptr_sasl_->ExtractAuthenticationMechanismResponse();
    std::string expected_response = first_chunk + second_chunk;
    EXPECT_STREQ(extracted_response.str().c_str(), expected_response.c_str());
}

TEST_F(SASLTest, ParsesAuthenticationMechanisms) {
    std::string response = authentication_mechanism_response();
    std::string payload = response.substr(response.find("<stream:features>"));
    std::stringstream ss;
    ss << payload;

    std::string plain = "PLAIN";
    std::string md5 = "DIGEST-MD5";
    std::string oauth = "X-OAUTH2";
    std::string sha1 = "SCRAM-SHA-1";
    std::unordered_set<std::string> authentication_mechanism_set = ptr_sasl_->ParseAuthenticationMechanisms(ss);
    EXPECT_TRUE(authentication_mechanism_set.find(plain) != authentication_mechanism_set.end());
    EXPECT_TRUE(authentication_mechanism_set.find(md5) != authentication_mechanism_set.end());
    EXPECT_TRUE(authentication_mechanism_set.find(oauth) != authentication_mechanism_set.end());
    EXPECT_TRUE(authentication_mechanism_set.find(sha1) != authentication_mechanism_set.end());
}

TEST_F(SASLTest, HandlesStreamInitialization) {
    EXPECT_EQ(ptr_sasl_->transaction_state(), SASL::TransactionState::kInitializeStream);

	ptr_sasl_->Update();
    EXPECT_EQ(ptr_sasl_->transaction_state(), SASL::TransactionState::kSelectAuthenticationMechanism);
}

TEST_F(SASLTest, HandlesAuthenticationMeachanismSelection) {
    std::string payload = authentication_mechanism_response();
	ptr_sasl_->set_transaction_state(SASL::TransactionState::kSelectAuthenticationMechanism);

    ptr_xmpp_client_->buffer() += payload;
	ptr_sasl_->Update();
    EXPECT_EQ(ptr_sasl_->transaction_state(), SASL::TransactionState::kDecodeBase64Challenge);
}

TEST_F(SASLTest, ParsesBase64Challenge) {
    std::string expected = "bm9uY2U9IjM0ODE5ODkxNDYyNDY2NTcyMjQiLHFvcD0iYXV0aCIsY2hhcnNldD11dGYtOCxhbGdvcml0aG09bWQ1LXNlc3M=";
    std::stringstream ss;
    ss << base64_challenge();

    std::string parsed_challenge = ptr_sasl_->ParseBase64Challenge(ss);
    EXPECT_STREQ(parsed_challenge.c_str(), expected.c_str());
}

TEST_F(SASLTest, DecodesBase64) {
    std::string input = "bm9uY2U9IjM0ODE5ODkxNDYyNDY2NTcyMjQiLHFvcD0iYXV0aCIsY2hhcnNldD11dGYtOCxhbGdvcml0aG09bWQ1LXNlc3M=";
    std::string expected = "nonce=\"3481989146246657224\",qop=\"auth\",charset=utf-8,algorithm=md5-sess";

    std::string decoded = ptr_sasl_->DecodeBase64(input);
    EXPECT_STREQ(decoded.c_str(), expected.c_str());
}

TEST_F(SASLTest, EncodesBase64) {
    std::string input = "nonce=\"3481989146246657224\",qop=\"auth\",charset=utf-8,algorithm=md5-sess";
    std::string expected = "bm9uY2U9IjM0ODE5ODkxNDYyNDY2NTcyMjQiLHFvcD0iYXV0aCIsY2hhcnNldD11dGYtOCxhbGdvcml0aG09bWQ1LXNlc3M=";

    std::string encoded = ptr_sasl_->EncodeBase64(input);
    EXPECT_STREQ(encoded.c_str(), expected.c_str());
}

TEST_F(SASLTest, InitiateAuthenticationStreamMD5) {
    std::string authentication_payload = ptr_sasl_->InitiateAuthenticationStream(SASL::Mechanism::kMD5).str();
    std::string expected = "<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>";
    EXPECT_STREQ(authentication_payload.c_str(), expected.c_str());
}

TEST_F(SASLTest, InitiateAuthenticationStreamOther) {
    std::string authentication_payload = ptr_sasl_->InitiateAuthenticationStream(SASL::Mechanism::kNone).str();
    std::string expected = "<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='PLAIN'/>";
    EXPECT_STREQ(authentication_payload.c_str(), expected.c_str());
}

TEST_F(SASLTest, InitiateAuthenticationStreamSHA1) {
    std::string authentication_payload = ptr_sasl_->InitiateAuthenticationStream(SASL::Mechanism::kSHA1).str();
    EXPECT_TRUE(authentication_payload.find("SCRAM-SHA-1") != std::string::npos);

    size_t initial_message_start = authentication_payload.find('>');
    size_t initial_message_end = authentication_payload.find('<');
	initial_message_end = authentication_payload.find('<', initial_message_end + 1);
    size_t expected_size = 60;
	std::string expected_first_characters = "biws";
    std::string initial_message = authentication_payload.substr(initial_message_start + 1, initial_message_end - initial_message_start - 1);
    EXPECT_EQ(initial_message.size(), expected_size);
	EXPECT_STREQ(initial_message.substr(0, 4).c_str(), expected_first_characters.c_str());
}

}
}
