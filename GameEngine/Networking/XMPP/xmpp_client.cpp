#include "xmpp_client.h"
#include "tcp_socket.h"
#include "host_resolver.h"

namespace GameEngine {
namespace Networking {

XMPPClient::XMPPClient(const std::string& xmpp_host, const std::string& xmpp_service, TCPClient& tcp_client):
    xmpp_host_(xmpp_host), xmpp_service_(xmpp_service), tcp_client_(tcp_client) { }

void XMPPClient::Connect() {
    std::string data_to_send_on_connect = "First part of SASLHandshake";
    tcp_client_.Connect(xmpp_host_, xmpp_service_, data_to_send_on_connect);
}

}
}
