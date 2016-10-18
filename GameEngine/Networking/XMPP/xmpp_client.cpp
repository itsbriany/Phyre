#include "xmpp_client.h"
#include "tcp_socket.h"
#include "host_resolver.h"

namespace GameEngine {
namespace Networking {

XMPPClient::XMPPClient(boost::asio::io_service& io_service): TCPClient(io_service) { }

void XMPPClient::OnConnect() {
    std::ostringstream data_to_send_on_connect;
    data_to_send_on_connect << "First part of SASLHandshake";
    Write(data_to_send_on_connect);
}

}
}
