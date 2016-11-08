#include <boost/property_tree/xml_parser.hpp>
#include "sasl.h"

namespace GameEngine {
namespace Networking {

SASL::SASL(XMPPClient& client): XMPPState(client), transaction_state_(kInitializeStream) { }

void SASL::Update() {
    switch (transaction_state_) {
        case TransactionState::kInitializeStream:
            HandleInitializeStream();
            break;
        case TransactionState::kSelectAuthenticationMechanism:
            HandleSelectAuthenticationMechanism();
            break;
        case TransactionState::kDecodeBase64Challenge:
            HandleDecodeBase64Challenge();
            break;
        default:
            Logging::error("Unknown Transaction State", *this);
            client_.Disconnect();
            return;
    }
}

void SASL::HandleInitializeStream() {
    client_.Write(initiation_stream());
    transaction_state_ = TransactionState::kSelectAuthenticationMechanism;
}

void SASL::HandleSelectAuthenticationMechanism() {
    std::stringstream authentication_mechanism_response = ExtractAuthenticationMechanismResponse();
    if (authentication_mechanism_response.str().empty())
        return;

    std::unordered_set<std::string> authentication_mechanism_set = ParseAuthenticationMechanisms(authentication_mechanism_response);
    if (!IsMD5AuthenticationMechanismAvailable(authentication_mechanism_set)) {
        Logging::error("Only MD5 SASL authentication mechanism is supported", *this);
        client_.Disconnect();
        return;
    }

    transaction_state_ = TransactionState::kDecodeBase64Challenge;
    Logging::debug("Sending authentication mechanism...", *this);
    client_.Write(authentication_mechanism());
}

void SASL::HandleDecodeBase64Challenge() {
    std::string start_tag = "<challenge";
    std::string end_tag = "</challenge>";
    std::stringstream extracted_response = ExtractXML(start_tag, end_tag);
    if (extracted_response.str().empty()) {
        return;
    }
    std::string base64_challenge = ParseBase64Challenge(extracted_response);
    std::string decoded_challenge = DecodeBase64(base64_challenge);
}

std::string SASL::ParseBase64Challenge(std::istream& xml_stream) {
    boost::property_tree::ptree pt;
    read_xml(xml_stream, pt);
    boost::property_tree::ptree::const_iterator it = pt.begin();
    return it->second.get_value<std::string>();
}

std::string SASL::DecodeBase64(const std::string& input) {
    std::istringstream iss(input);
    std::ostringstream oss;
    base64_decoder_.decode(iss, oss);
    return oss.str();
}

bool SASL::IsMD5AuthenticationMechanismAvailable(const std::unordered_set<std::string>& authentication_mechanism_set) {
    std::string md5 = "DIGEST-MD5";
    return authentication_mechanism_set.find(md5) != authentication_mechanism_set.end();
}

std::stringstream SASL::ExtractXML(const std::string& start_tag, const std::string& end_tag) {
    std::stringstream extracted_response;
    size_t found_start = client_.buffer().find(start_tag);
    if (found_start == std::string::npos) {
        return extracted_response;
    }

    size_t found_end = client_.buffer().find(end_tag);
    if (found_end == std::string::npos) {
        return extracted_response;
    }

    size_t bytes_to_extract = found_end + end_tag.size() - found_start;
    extracted_response << client_.buffer().substr(found_start, bytes_to_extract);
    client_.buffer() = client_.buffer().substr(found_end + end_tag.size());
    return extracted_response;
}

std::stringstream SASL::ExtractAuthenticationMechanismResponse() {
    std::string search_start = "<stream:features>";
    std::string search_end = "</stream:features>";
    return ExtractXML(search_start, search_end);
}

std::unordered_set<std::string> SASL::ParseAuthenticationMechanisms(std::istream& xml_stream) {
    boost::property_tree::ptree pt;
    std::unordered_set<std::string> authentication_mechanism_set;
    read_xml(xml_stream, pt);
    ParseAuthenticationMechanismsRecursive(pt, authentication_mechanism_set);
    return authentication_mechanism_set;
}

void SASL::ParseAuthenticationMechanismsRecursive(const boost::property_tree::ptree& pt,
                                                        std::unordered_set<std::string>& authentication_mechanism_set) {
    for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it) {
        if (it->first == "mechanism") {
            std::string mechanism_value = it->second.get_value<std::string>();
            authentication_mechanism_set.insert(mechanism_value);
        }
        ParseAuthenticationMechanismsRecursive(it->second, authentication_mechanism_set);
    }
}

std::ostringstream SASL::xml_version() {
    std::ostringstream oss;
    oss << "<?xml version='1.0'?>";
    return oss;
}

std::ostringstream SASL::initiation_stream() {
    std::ostringstream oss;
    oss << xml_version().str();
    oss << "<stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' to='localhost' version='1.0'>";
    return oss;
}

std::ostringstream SASL::authentication_mechanism() {
    std::ostringstream oss;
    oss << "<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>";
    return oss;
}

}
}
