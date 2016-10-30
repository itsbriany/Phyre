#include "xmpp_client.h"
#include "tcp_socket.h"
#include "host_resolver.h"

namespace GameEngine {
namespace Networking {

XMPPClient::XMPPClient(boost::asio::io_service& io_service):
    TCPClient(io_service),
    state_(TransactionState::kSelectAuthenticationMechanism) { }

void XMPPClient::OnConnect() {
    std::ostringstream data_to_send_on_connect;
    data_to_send_on_connect << initiation_stream();
    Write(data_to_send_on_connect);
}

std::string XMPPClient::xml_version() {
    return "<?xml version='1.0'?>";
}

std::string XMPPClient::initiation_stream() {
    std::ostringstream oss;
    oss << xml_version();
    oss << "<stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' to='localhost' version='1.0'/>";
    return oss.str();
}

std::string XMPPClient::authentication_mechanism() {
    std::ostringstream oss;
    oss << xml_version();
    oss << "<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>";
    return oss.str();
}

}
}
