#include <boost/property_tree/xml_parser.hpp>
#include "xmpp_client.h"
#include "tcp_socket.h"
#include "host_resolver.h"
#include "logging.h"

namespace GameEngine {
namespace Networking {

XMPPClient::XMPPClient(boost::asio::io_service& io_service):
    TCPClient(io_service),
    state_(TransactionState::kSelectAuthenticationMechanism) { }

void XMPPClient::OnConnect() {
    Write(initiation_stream());
}

void XMPPClient::OnRead(const std::string& bytes_read) {
    std::string message = "From server: " + std::string(bytes_read);
    Logging::info(message, *this);
    switch (state_) {
        case TransactionState::kSelectAuthenticationMechanism:
            HandleSelectAuthenticationMechanism(bytes_read);
            break;
        default:
            Logging::error("Unknown TransactionState", *this);
            Disconnect();
            return;
    }
}

void XMPPClient::OnError(const boost::system::error_code& ec) {
    Logging::error(ec.message(), *this);
}

void XMPPClient::HandleSelectAuthenticationMechanism(const std::string& bytes_read) {
    std::stringstream authentication_mechanism_response = ExtractAuthenticationMechanismResponse(bytes_read);
    if (authentication_mechanism_response.str().empty())
        return;

    std::unordered_set<std::string> authentication_mechanism_set = ParseAuthenticationMechanisms(authentication_mechanism_response);
    if (!IsMD5AuthenticationMechanismAvailable(authentication_mechanism_set)) {
        Logging::error("Only MD5 SASL authentication mechanism is supported", *this);
        Disconnect();
        return;
    }

    state_ = TransactionState::kDecodeBase64Challenge;
    Write(authentication_mechanism());
}

bool XMPPClient::IsMD5AuthenticationMechanismAvailable(const std::unordered_set<std::string>& authentication_mechanism_set) {
    std::string md5 = "DIGEST-MD5";
    return authentication_mechanism_set.find(md5) != authentication_mechanism_set.end();
}

std::stringstream XMPPClient::ExtractAuthenticationMechanismResponse(const std::string& bytes_read) {
    buffer_ += bytes_read;
    std::stringstream extracted_response;
    std::string search = "</stream:stream>";
    size_t found = buffer_.find(search);
    if (found == std::string::npos) {
        return extracted_response;
    }

    extracted_response << buffer_.substr(0, found + search.size());
    buffer_ = buffer_.substr(found + search.size());
    return extracted_response;
}

std::unordered_set<std::string> XMPPClient::ParseAuthenticationMechanisms(std::istream& xml_stream) {
    boost::property_tree::ptree pt;
    std::unordered_set<std::string> authentication_mechanism_set;
    read_xml(xml_stream, pt);
    ParseAuthenticationMechanismsRecursive(pt, authentication_mechanism_set);
    return authentication_mechanism_set;
}

void XMPPClient::ParseAuthenticationMechanismsRecursive(const boost::property_tree::ptree& pt,
                                                        std::unordered_set<std::string>& authentication_mechanism_set) {
    for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it) {
        if (it->first == "mechanism") {
            std::string mechanism_value = it->second.get_value<std::string>();
            authentication_mechanism_set.insert(mechanism_value);
        }
        ParseAuthenticationMechanismsRecursive(it->second, authentication_mechanism_set);
    }
}

std::ostringstream XMPPClient::xml_version() {
    std::ostringstream oss;
    oss << "<?xml version='1.0'?>";
    return oss;
}

std::ostringstream XMPPClient::initiation_stream() {
    std::ostringstream oss;
    oss << xml_version().str();
    oss << "<stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' to='localhost' version='1.0'/>";
    return oss;
}

std::ostringstream XMPPClient::authentication_mechanism() {
    std::ostringstream oss;
    oss << xml_version().str();
    oss << "<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>";
    return oss;
}

}
}
