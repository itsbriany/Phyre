#include <boost/property_tree/xml_parser.hpp>
#include "host_resolver.h"
#include "logging.h"
#include "sasl.h"
#include "tcp_socket.h"
#include "xmpp_client.h"

namespace GameEngine {
namespace Networking {

XMPPClient::XMPPClient(boost::asio::io_service& io_service):
    TCPClient(io_service), p_state_(std::make_unique<SASL>(*this)) { }

void XMPPClient::OnConnect() {
    p_state_->Update();
}

void XMPPClient::OnRead(const std::string& bytes_read) {
    std::string message = "From server: \n" + std::string(bytes_read);
    Logging::debug(message, *this);
    buffer_ += bytes_read;
    p_state_->Update();
}

void XMPPClient::OnError(const boost::system::error_code& ec) {
    Logging::error(ec.message(), *this);
}

}
}
