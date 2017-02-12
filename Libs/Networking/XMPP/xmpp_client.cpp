#include "logging.h"
#include "sasl.h"
#include "xmpp_client.h"

namespace Phyre {
namespace Networking {

XMPPClient::XMPPClient(boost::asio::io_service& io_service,
                       const std::string& host,
                       const std::string& username,
                       const std::string& password):
        TCPClient(io_service),
        host_(host),
        username_(username),
        password_(password),
        ptr_state_(std::make_unique<SASL>(*this)) { }

void XMPPClient::OnConnect() {
    ptr_state_->Update();
}

void XMPPClient::OnRead(const std::string& bytes_read) {
    std::string message = "From server: \n" + std::string(bytes_read);
    Logging::debug(message, *this);
    buffer_ += bytes_read;
    ptr_state_->Update();
}

void XMPPClient::OnError(const boost::system::error_code& ec) {
    Logging::error(ec.message(), *this);
}

}
}
